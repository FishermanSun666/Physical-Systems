#pragma once
#include "PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {
		class GameBall : public GameObject {
		protected:
			float g_max_angle = 3.0f;
			float g_max_force = 20.0f;

			float force = 0.0f;
			float angleAmend = 0.0f;
			Vector3 originPos;
		public:
			GameBall(const std::string name) : GameObject(name) {}
			~GameBall() {}
			void SetOriginPosition(Vector3 pos) {
				originPos = pos;
			}
			void Reset() {
				force = 0.0f;
				angleAmend = 0.0f;
				GetTransform().SetPosition(originPos);
				GetPhysicsObject()->SetLinearVelocity(Vector3(0.0f, 0.0f, 0.0f));
			}
			void AddKickAngle(float y) {
				float temp = angleAmend + y;
				if (temp > g_max_angle) {
					temp = g_max_angle;
				}
				if (temp < 0) {
					temp = 0;
				}
				angleAmend = temp;
			}
			void AddForce(float dt) {
				float temp = force + dt;
				if (temp > g_max_force) {
					temp = g_max_force;
				}
				force = temp;
			}
			void Kick(Vector3 dir) {
				//dir.y += angleAmend;
				dir.y += angleAmend;
				GetPhysicsObject()->ClearForces();
				GetPhysicsObject()->SetLinearVelocity(dir * force);
				//GetPhysicsObject()->ApplyLinearImpulse(dir * force);
				//GetPhysicsObject()->AddForceAtPosition(dir * force, GetTransform().GetPosition());
			}

			float GetAngleAmend() { return angleAmend; }
			float GetForce() { return force; }
		};
	}
}