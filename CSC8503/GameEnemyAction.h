#pragma once
#include "BehaviourNode.h"
#include "GameEnemy.h"

class GameEnemy;

typedef std::function<BehaviourState(float, Vector3 , GameEnemy*, BehaviourState)> GameEnemyActionFunc;

class GameEnemyAction : public BehaviourNode {
public:
	GameEnemyAction(const std::string& nodeName) : BehaviourNode(nodeName) {}
	GameEnemyAction(Vector3 tg, GameEnemy* obj, const std::string& nodeName, GameEnemyActionFunc func) : BehaviourNode(nodeName) {
		target = tg;
		object = obj;
		execute = func;
	}

	BehaviourState Execute(float dt) override {
		currentState = execute(dt, target, object, currentState);
		return currentState;
	}

	GameEnemy* GetGameEnemy() { return object; }

protected:
	Vector3 target;
	GameEnemy* object;
	GameEnemyActionFunc execute;;
};