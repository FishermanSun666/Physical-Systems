#pragma once
#include "PhysicsSystem.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

namespace NCL {
	namespace CSC8503 {
#define PLAYER_LIVE_TIME 60.0f
#define PLAYER_RIVIVE_TIME 5.0f
		class PositionConstraint;
		class GamePlayer : public GameObject {
		protected:
			bool prapare = true;
			int score = 0;
			float liveTime = PLAYER_LIVE_TIME;
			float reviveTime = 3.0f;
			float speed = 8.0f;
			float quickSpeed = 15.0f;
			float turnSpeed = 0.02;

			Vector3 originPos;
			StateMachine* stateManager;
			PositionConstraint* catchBall;
		public:
			GamePlayer(const std::string name) : GameObject(name) {
				stateManager = new StateMachine();
				State* reviveTiming = new State(
					[&](float dt)->void {
						if (!prapare) {
							liveTime = liveTime < dt ? 0.0f : liveTime - dt; // prapare time
						}
						reviveTime -= dt;
					});
				State* liveTiming = new State(
					[&](float dt)->void {
						liveTime = liveTime < dt? 0.0f : liveTime - dt;
					});
				
				StateTransition* revive = new StateTransition(liveTiming, reviveTiming,
					[&](void)->bool {
						return reviveTime != 0;
					});
				StateTransition* live = new StateTransition(reviveTiming, liveTiming,
					[&](void)->bool {
						if (reviveTime <= 0) {
							//prapare time
							if (prapare) {
								prapare = false;
							}
							return true;
						}
						return false;
					});
				stateManager->AddState(reviveTiming);
				stateManager->AddState(liveTiming);
				stateManager->AddTransition(revive);
				stateManager->AddTransition(live);
			}
			~GamePlayer() {
				if (nullptr != stateManager) {
					delete stateManager;
				}
				if (nullptr != catchBall) {
					delete catchBall;
				}
			}

			void AddScore(int s) {
				score += s;
			}
			void SetOriginPosition(Vector3 pos) {
				originPos = pos;
			}
			void SetReviveTime(float t) {
				reviveTime = t;
			}
			void Update(float dt) {
				stateManager->Update(dt);
			}
			void Revive() {
				reviveTime = PLAYER_RIVIVE_TIME;
				GetTransform().SetPosition(originPos);
				GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			}

			bool CheckCatchBall() { return catchBall != nullptr; }
			bool GameOver() { return 0 >= liveTime; }
			bool InReviveTime() { return reviveTime > 0; }
			int GetScore() { return score; }
			float GetLiveTime() { return liveTime; }
			float GetReviveTime() { return reviveTime; }
			float GetSpeed() { return speed; }
			float GetQuickSpeed() { return quickSpeed; }
			float GetTurnSpeed() { return turnSpeed; }

			void CatchBall(PositionConstraint* cb) {
				catchBall = cb;
			}
			PositionConstraint* LostBall() {
				PositionConstraint* ret = catchBall;
				catchBall = nullptr;
				return ret;
			}
		};
	}
}