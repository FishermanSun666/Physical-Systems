#pragma once
#include "PushdownState.h"
#include "GameEnemyAction.h"

using namespace PhysicalProject::GameDemo;

class GameEnemyTrackingPlayer : public PushdownState {
public:
	GameEnemyTrackingPlayer(GameEnemyAction* ac) : PushdownState() {
		action = ac;
	}
	PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override {
		//if u want add more higher priority action, + here
		//return PushdownResult::Push;
		if (Ongoing != action->Execute(dt)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "start tracking player..." << std::endl;
	}
	void OnSleep() override {
		action->Reset();
		std::cout << "stop tracking player" << std::endl;
	}

protected:
	GameEnemyAction* action;
};