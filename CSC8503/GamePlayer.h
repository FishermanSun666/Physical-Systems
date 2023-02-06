#pragma once
#include "PhysicsSystem.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

namespace NCL {
	namespace CSC8503 {
		class PositionConstraint;
		class GamePlayer : public GameObject {
		protected:
			float g_game_live_time = 60.0f;

			bool prapare = true;
			int score = 0;
			float liveTime = g_game_live_time;
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
							liveTime -= dt;
						}
						reviveTime -= dt;
					});
				State* liveTiming = new State(
					[&](float dt)->void {
						liveTime -= dt;
					});
				
				StateTransition* revive = new StateTransition(liveTiming, reviveTiming,
					[&](void)->bool {
						return reviveTime != 0;
					});
				StateTransition* live = new StateTransition(reviveTiming, liveTiming,
					[&](void)->bool {
						bool ret = reviveTime <= 0;
						if (ret) {
							//prapare time
							if (prapare) {
								prapare = false;
							}
						}
						return ret;
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
			void Revive(float tm) {
				reviveTime = tm;
				GetTransform().SetPosition(originPos);
				GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			}

			bool KeepBall() { return catchBall != nullptr; }
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