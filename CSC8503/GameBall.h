#pragma once
#include "PhysicsSystem.h"

namespace PhysicalProject {
	namespace GameDemo {
		class GameBall : public GameObject {
		protected:
			const float BALL_MAX_ANGLE = 2.0f;
			const float BALL_MAX_FORCE = 8000.0f;

			float force = 0.0f;
			float angleAmend = 0.0f;
			Vector3 originPos;
			Vector3 kickedPos;
		public:
			GameBall(const std::string name) : GameObject(name) {}
			~GameBall() {}
			void SetOriginPosition(Vector3 pos) {
				originPos = pos;
			}
			Vector3 GetKickedPosition() { return kickedPos; }
			void Reset() {
				force = 0.0f;
				angleAmend = 0.0f;
				GetTransform().SetPosition(originPos);
				GetPhysicsObject()->SetLinearVelocity(Vector3(0.0f, 0.0f, 0.0f));
			}
			void AddKickAngle(float y) {
				angleAmend = angleAmend + y > BALL_MAX_ANGLE ? BALL_MAX_ANGLE : angleAmend + y;
			}
			void IncreaseForce(float dt) {
				force = force + dt > BALL_MAX_FORCE ? BALL_MAX_FORCE : force + dt;
			}
			void DecreaseForce(float dt) { force = force - dt < 0.0f ? 0.0f : force - dt; }
			void Kick(Vector3 dir) {
				//dir.y += angleAmend;
				kickedPos = transform.GetPosition();
				dir.y = angleAmend;
				GetPhysicsObject()->ClearForces();
				GetPhysicsObject()->AddForce(dir * force);
			}

			float GetAngleAmend() { return angleAmend; }
			float GetForce() { return force; }
		};
	}
}