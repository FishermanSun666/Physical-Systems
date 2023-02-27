#include "PushdownMachine.h"
#include "PushdownState.h"

using namespace NCL::GameDemo;

PushdownMachine::PushdownMachine(PushdownState* initialState)
{
	this->initialState = initialState;
}

PushdownMachine::~PushdownMachine()
{
	while (!stateStack.empty()) {
		auto dl = stateStack.top();
		stateStack.pop();
		delete dl;
	}
	while (!reserveStack.empty()) {
		auto dl = reserveStack.top();
		reserveStack.pop();
		delete dl;
	}
}

void PushdownMachine::AddReserveState(PushdownState* state) {
	reserveStack.push(state);
}

bool PushdownMachine::Update(float dt) {
	if (activeState) {
		PushdownState* newState = nullptr;
		PushdownState::PushdownResult result = activeState->OnUpdate(dt, &newState);

		switch (result) {
			case PushdownState::Pop: {
				activeState->OnSleep();
				reserveStack.push(activeState);
				//delete activeState;
				stateStack.pop();
				if (stateStack.empty()) {
					return false;
				}
				else {
					activeState = stateStack.top();
					activeState->OnAwake();
				}					
			}break;
			case PushdownState::Push: {
				activeState->OnSleep();		

				if (newState == nullptr) {
					//if state not support new state, get new state from reserve
					if (reserveStack.empty()) {
						return false;
					}
					newState = reserveStack.top();
					reserveStack.pop();
				}
				stateStack.push(newState);
				activeState = newState;
				activeState->OnAwake();
			}break;
		}
	}
	else {
		stateStack.push(initialState);
		activeState = initialState;
		activeState->OnAwake();
	}
	return true;
}