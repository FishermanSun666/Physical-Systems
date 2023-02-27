#pragma once
#include "PhysicsSystem.h"
#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

namespace NCL {
	namespace GameDemo {
		const float PLAYER_LIVE_TIME = 60.0f;
		const float PLAYER_RIVIVE_TIME = 5.0f;
		const float PLAYER_SPEED_1 = 25.0f;
		const float PLAYER_SPEED_2 = 50.0f;
		const float PLAYER_SPEED_UP_LIMIT = 3.0f; //seconds
		const float PLAYER_TURN_SPEED = 0.02f;
		const Vector4 PLAYER_DEFAULT_COLOUR = Vector4(1.5f, 1.5f, 1.5f, 2.0f);

		class PositionConstraint;
		class GamePlayer : public GameObject {
		protected:
			bool prapare = true;
			int score = 0;
			float liveTime = PLAYER_LIVE_TIME;
			float restrictedTime = 3.0f;
			float speed = PLAYER_SPEED_1;
			float quickSpeed = PLAYER_SPEED_2;
			float turnSpeed = PLAYER_TURN_SPEED;
			float speedUpTime = 0.0f;

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
						restrictedTime -= dt;
					});
				State* liveTiming = new State(
					[&](float dt)->void {
						liveTime = liveTime < dt? 0.0f : liveTime - dt;
					});
				
				StateTransition* revive = new StateTransition(liveTiming, reviveTiming,
					[&](void)->bool {
						return restrictedTime != 0;
					});
				StateTransition* live = new StateTransition(reviveTiming, liveTiming,
					[&](void)->bool {
						if (restrictedTime <= 0) {
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
				restrictedTime = t;
			}
			void Update(float dt) {
				stateManager->Update(dt);
			}
			void Revive() {
				restrictedTime = PLAYER_RIVIVE_TIME;
				GetTransform().SetPosition(originPos);
				GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			}

			bool CheckCatchBall() { return catchBall != nullptr; }
			bool CheckGameOver() { return 0 >= liveTime; }
			bool CheckInReviveTime() { return restrictedTime > 0; }

			int GetScore() { return score; }
			float GetLiveTime() { return liveTime; }
			float GetReviveTime() { return restrictedTime; }
			float GetSpeed() { return speed; }
			float GetQuickSpeed() { return quickSpeed; }
			float GetTurnSpeed() { return turnSpeed; }

			bool SpeedUp(float dt) {
				if (speedUpTime >= PLAYER_SPEED_UP_LIMIT) { return false; }
				speedUpTime += dt;
				return true;
			}

			void RecoverStrength(float dt) {
				speedUpTime = speedUpTime - dt < 0.0f ? 0.0f : speedUpTime - dt;
			}

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