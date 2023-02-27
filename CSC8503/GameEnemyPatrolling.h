#pragma once
#include "PushdownState.h"
#include "GameEnemyAction.h"
#include "BehaviourSequence.h"
#include "GameEnemyTrackingPlayer.h"

using namespace PhysicalProject::GameDemo;

class GameEnemyPatrolling : public PushdownState{
public:
	GameEnemyPatrolling(BehaviourSequence* ac) : PushdownState() {
		action = ac;
	}
	PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override {
		//base, don't pop
		//return PushdownResult::Pop;
		BehaviourState state = Ongoing;
		state = action->Execute(dt);
		if (state != Ongoing) {
			if (state == Failure) {
				//Tracking Player
				return PushdownResult::Push;
			}
			action->Reset();
		}
		return PushdownResult::NoChange;
	}

	void OnSleep() override {
		action->Reset();
	}

protected:
	BehaviourSequence* action;
};