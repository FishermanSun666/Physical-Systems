#pragma once
#include "BehaviourNodeWithChildren.h"

class ParallelBehaviour : public BehaviourNodeWithChildren {
public:
	ParallelBehaviour(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
	~ParallelBehaviour() {}
	BehaviourState Execute(float dt) override {
		//std::cout << "Executing Parallel " << name << "\n";
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);
			switch (nodeState) {
			case Failure:
			case Success:
			{
				//This behaviour will be successful if one child success.
				if (Success != currentState) {
					currentState = nodeState;
				}
			continue;
			}
			case Ongoing:
			{
				currentState = nodeState;
				return currentState;
			}
			}
		}
		return currentState;
	}
};