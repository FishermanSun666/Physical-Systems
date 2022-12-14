#pragma once
#include "PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {
		class GamePlayer : public GameObject {
		private:
			float reviveTime = 0.0f;
			float speed = 1.0f;
		public:
			GamePlayer() {};
			~GamePlayer() {};

			float GetReviveTime() { return reviveTime; }
			float GetSpeed() { return speed; }
		};
	}
}