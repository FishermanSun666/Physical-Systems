#pragma once
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "NavigationGrid.h"
#include "BehaviourSequence.h"
#include "PushdownMachine.h"
#include <corecrt_math_defines.h>
#include "GamePlayer.h"
#include "GameEnemyAction.h"
#include "GameEnemyPatrolling.h"
#include "../NCLCoreClasses/Maths.h"

using namespace PhysicalProject;
using namespace GameDemo;

const float ENEMY_SIGHT_LINE = 50.0f;
const float ENEMY_SIGHT_RANGE_DEGREE = 60.0f; //Fan view
const float ENEMY_SPEED = 150.0f;
const float	ENEMY_TRACKING_SPEED = 200.0f;
const float ENEMY_ARRIVE_OFFSET = 2.0f;
const Vector4 ENEMY_DEFAULT_COLOUR = Vector4(3.0f, 3.0f, 3.0f, 1.0f);
const Vector4 ENEMY_TRACKING_COLOUR = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

class Debug;
class PhysicsObject;

class GameEnemy : public GameObject {
private:

	float speed = ENEMY_SPEED;
	//pathFinding
	Vector3 lastTarget;
	Vector3 moveTarget;
	Vector3 playerPosition;
	NavigationGrid* map;
	//behaviour tree
	PushdownMachine* aiManager;

public:
	GameEnemy(const std::string name) : GameObject(name) {};
	~GameEnemy() {};

	Vector3 GetPlayerPosition() { return playerPosition; }

	void SetPathFindingMap(NavigationGrid* m) {
		map = m;
	}
	void StartTrackingPlayer(Vector3 pp) {
		playerPosition = pp;
		speed = ENEMY_TRACKING_SPEED;
		SetColour(ENEMY_TRACKING_COLOUR);
	}
	void LostPlayer() {
		playerPosition = Vector3();
		speed = ENEMY_SPEED;
		SetColour(ENEMY_DEFAULT_COLOUR);
	}
	bool TrackingPlayer() {
		return (playerPosition != Vector3());
	}

	bool PathFinding(const Vector3& target) {
		if (nullptr == map) { return false; }
		Vector3 position = transform.GetPosition();
		//find path
		NavigationPath outPath;
		bool found = map->FindPath(position, target, outPath);
		if (!found) { return false; }
		//get next target
		Vector3 toPos;
		outPath.PopWaypoint(moveTarget); //pos first position
		Vector3 pre = position;
		bool find = false;
		while (outPath.PopWaypoint(toPos)) {
			toPos.y = position.y;
			if (!find) {
				moveTarget = toPos;
				find = true;
			}
			PhysicalProject::Debug::DrawLine(pre, toPos, Vector4(0, 1, 0, 1));
			pre = toPos;
		}

		return true;
	};

	void Move(float dt) {
		Vector3 position = transform.GetPosition();
		Vector3 path = position - moveTarget;

		//turn orientation
		Matrix4 temp = Matrix4::BuildViewMatrix(position, moveTarget, Vector3(0, 1, 0));
		Matrix4 modelMat = temp.Inverse();
		Quaternion q(modelMat);
		transform.SetOrientation(q);
		//move
		path.Normalise();
		path.y = 0;
		physicsObject->AddForce(-path * speed);
	}

	bool MoveToTarget(float dt, Vector3 target) {
		//arrive
		Vector3 pos = GetTransform().GetPosition();
		Vector3 tmp = Vector3(target.x, pos.y, target.z);
		if (ENEMY_ARRIVE_OFFSET >= Maths::Distance(pos, tmp)) {
			lastTarget = target;
			return false;
		}
		PathFinding(tmp);
		Move(dt);
		return true;
	}

	void InitialiseBehaviours(std::vector<Vector3> targets) {
		if (0 == targets.size()) {
			return;
		}
		//make putdown machine
		//patrolling action
		auto patrolling = new GameEnemyPatrolling(NewPatrollingAction(targets));
		auto tracking = new GameEnemyTrackingPlayer(NewTrackingPlayerAction());
		aiManager = new PushdownMachine(patrolling);
		aiManager->AddReserveState(tracking);
	}
	//make patrolling action
	BehaviourSequence* NewPatrollingAction(std::vector<Vector3> targets) {
		if (0 == targets.size()) {
			std::cout << "patrol target not exist!" << std::endl;
			return nullptr;
		}
		BehaviourSequence* sequence = new BehaviourSequence("Patrolling Sequence");
		for (auto& i : targets) {
			GameEnemyAction* patrolling = new GameEnemyAction(i, this, "Patrolling...",
				[&](float dt, Vector3 target, GameEnemy* object, BehaviourState state)->BehaviourState {
					//track player
					if (object->TrackingPlayer()) { return Failure; }
					if (state == Initialise) { return Ongoing; }
					if (state == Ongoing) {
						if (lastTarget == target) { return Success; }
						if (!object->MoveToTarget(dt, target)) { return Success; }
						return state;
					}
					return state;
				});
			sequence->AddChild(patrolling);
		}
		return sequence;
	}
	//make tracking action
	GameEnemyAction* NewTrackingPlayerAction() {
		GameEnemyAction* tracking = new GameEnemyAction(playerPosition, this, "Tracking Player...", 
			[&](float dt, Vector3 target, GameEnemy* object, BehaviourState state)->BehaviourState {
				//tacking player
				if (state == Initialise) { return Ongoing; }
				if (state == Ongoing) {
					//lost target
					if (!object->TrackingPlayer()) { return Failure; }
					if (!object->MoveToTarget(dt, object->GetPlayerPosition())) {
						object->LostPlayer();
						return Success;
					}
					return state;
				}
				return state;
			});
		return tracking;
	}

	void UpdateAction(float dt) {
		if (aiManager == nullptr) {
			return;
		}
		aiManager->Update(dt);
	}

	bool CheckPositionInView(Vector3 pos) {
		Vector3 vecEtoP = pos - transform.GetPosition();
		float distance = PhysicalProject::Maths::Distance(pos, transform.GetPosition());
		if (distance > ENEMY_SIGHT_LINE) { return false; } //over sight
		Vector3 dirE = transform.GetOrientation() * Vector3(0, 0, -1); // positive direction
		float cosVal = Vector3::Dot(vecEtoP.Normalised(), dirE.Normalised());
		float degree = acos(cosVal) * 180.0f / M_PI;
		if (degree > ENEMY_SIGHT_RANGE_DEGREE) { return false; }
		return true;
	}
};
