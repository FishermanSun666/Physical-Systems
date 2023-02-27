#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "Camera.h"
#include "../NCLCoreClasses/Maths.h"
#include <corecrt_math_defines.h>


using namespace NCL;
using namespace NCL::GameDemo;

GameWorld::GameWorld()	{
	mainCamera = new Camera();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

void GameWorld::ClearAndErase() {
	gameObjects = std::vector<GameObject*>();
	for (auto i : gameObjects) {
		delete i;
	}
	for (auto i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	worldStateCounter++;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
	worldStateCounter++;
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	auto rng = std::default_random_engine{};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);

	if (shuffleObjects) {
		std::shuffle(gameObjects.begin(), gameObjects.end(), e);
	}

	if (shuffleConstraints) {
		std::shuffle(constraints.begin(), constraints.end(), e);
	}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, GameObject* ignoreThis) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be colliedable etc...
			continue;
		}
		if (i == ignoreThis) {
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {
				
			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}

//Get all objects oriented in the direction of this vector
//@param objects - return
//@param pos - start point
//@param vec - direction
void GameWorld::GetObjectsInTargetView(vector<GameObject*> &objects, Vector3 pos, Vector3 vec, float distance) {
	for (auto object : gameObjects) {
		if (object->GetName() == "Player" || object->GetName() == "Enemy") { continue; }
		Vector3 ObjPos = object->GetTransform().GetPosition();
		Vector3 vecTtoO = ObjPos - pos;
		float lenTO = NCL::Maths::Distance(ObjPos, pos);
		if (distance < lenTO) { continue; } //too far
		float cosTOtoV = Vector3::Dot(vec.Normalised(), vecTtoO.Normalised());
		if (90.0f < acos(cosTOtoV) * 180.0f / M_PI) { continue; } // The angle with the vector should be less than 90 degree
		objects.push_back(object);
	}
}



////Get all objects in the square range between two objects
//void GameWorld::GetObjectSetRangeTwoObject(vector<GameObject*> objects, GameObject* objA, GameObject* objB) {
//	if (!objA || !objB) { return; }
//	Vector3 posA = objA->GetTransform().GetPosition();
//	Vector3 posB = objB->GetTransform().GetPosition();
//	for (auto obj : gameObjects) {
//		if (obj->GetWorldID() == objA->GetWorldID() || obj->GetWorldID() == objB->GetWorldID()) { continue; } //not include thenself;
//		auto posT = obj->GetTransform().GetPosition();
//		auto boundryMax = obj->GetBoundry() + posT;
//		auto boundryMin = posT - obj->GetBoundry();
//		if (CheckObjectInRangePos(boundryMax, boundryMin, posA, posB)) { objects.push_back(obj); }
//	}
//}
////Check if the object is between two position
//bool GameWorld::CheckObjectInRangePos(Vector3 boundryMax, Vector3 boundryMin, Vector3 posA, Vector3 posB) {
//	if (posA.x > posB.x) {
//		if (boundryMax.x < posB.x || boundryMin.x > posA.x) { return false; }
//		if (posA.z > posB.z) {
//			if (boundryMax.z < posB.z || boundryMin.z > posA.z) { return false; }
//			return true;
//		}
//		if (boundryMax.z < posA.z || boundryMin.z > posB.z) { return false; }
//		return true;
//	}
//	if (boundryMax.x < posA.x || boundryMin.x > posB.x) { return false; }
//	if (posA.z > posB.z) {
//		if (boundryMax.z < posB.z || boundryMin.z > posA.z) { return false; }
//		return true;
//	}
//	if (boundryMax.z < posA.z || boundryMin.z > posB.z) { return false; }
//	return true;
//}