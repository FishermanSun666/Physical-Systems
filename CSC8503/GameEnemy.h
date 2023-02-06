#pragma once
#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "NavigationGrid.h"
#include "BehaviourSequence.h"
//#include "BehaviourSelector.h"
#include "PushdownMachine.h"

#include "GamePlayer.h"
#include "GameEnemyAction.h"
#include "GameEnemyPatrolling.h"

using namespace NCL;
using namespace CSC8503;

class Debug;
class PhysicsObject;

class GameEnemy : public GameObject {
private:

	float g_enemy_speed1 = 10.0f;
	float g_enemy_arrive_offset = 2.0f;

	float speed = g_enemy_speed1;
	//pathFinding
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
	}
	void LostPlayer() {
		this->playerPosition = Vector3(0, 0, 0);
	}
	bool TrackingPlayer() {
		return (playerPosition != Vector3(0, 0, 0));
	}

	bool PathFinding(const Vector3& target) {
		if (nullptr == map) {
			std::cout << "map == null" << std::endl;
			return false;
		}
		Vector3 position = transform.GetPosition();
		//catch target
		if (g_enemy_arrive_offset * 4 >= (position - target).Length()) {
			moveTarget = target;
			return true;
		}

		//find path
		NavigationPath outPath;
		bool found = map->FindPath(position, target, outPath);
		if (!found) {
			return false;
		}

		//get next target
		Vector3 toPos;
		//TODO test
		//bool find = false;
		//Vector3 pre = position;
		//pop first position
		outPath.PopWaypoint(moveTarget);
		while (outPath.PopWaypoint(toPos)) {
			toPos.y = position.y;
			if (g_enemy_arrive_offset*2 >= (position - toPos).Length()) {
				//too close, find next position.
				continue;
			}
			//TODO test
			//if (!find) {
			//	moveTarget = toPos;
			//	find = true;
			//}
			moveTarget = toPos;
			break;
			//NCL::Debug::DrawLine(pre, toPos, Vector4(0, 1, 0, 1));
			//pre = toPos;
			//break;
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
		transform.SetPosition(transform.GetPosition() - path * speed * dt);
	}

	bool MoveToTarget(float dt, Vector3 target) {
		//arrive
		Vector3 pos = GetTransform().GetPosition();
		target.y = pos.y;
		if (g_enemy_arrive_offset >= (pos - target).Length()) {
			return false;
		}
		PathFinding(target);
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
					if (object->TrackingPlayer()) {
						return Failure;
					}
					if (state == Initialise) {
						return Ongoing;
					}
					if (state == Ongoing) {
						if (!object->MoveToTarget(dt, target)) {
							return Success;
						}
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
		GameEnemyAction* patrolling = new GameEnemyAction(playerPosition, this, "Tracking Player...", 
			[&](float dt, Vector3 target, GameEnemy* object, BehaviourState state)->BehaviourState {
				//tacking player
				if (state == Initialise) {
					std::cout << "start tracking player..." << std::endl;
					return Ongoing;
				}
				if (state == Ongoing) {
					//lost target
					if (!object->TrackingPlayer()) {
						return Failure;
					}
					if (!object->MoveToTarget(dt, object->GetPlayerPosition())) {
						object->LostPlayer();
						return Success;
					}
					return state;
				}
				return state;
			});
		return patrolling;
	}

	void UpdateBehaviour(float dt) {
		if (aiManager == nullptr) {
			return;
		}
		aiManager->Update(dt);
	}
};
