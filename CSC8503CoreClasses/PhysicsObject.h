#pragma once

using namespace PhysicalProject::Maths;

namespace PhysicalProject {
	class CollisionVolume;
	
	namespace GameDemo {
		class Transform;

		class PhysicsObject	{
		public:
			PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume);
			~PhysicsObject();

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void SetElasticity(float e) { elasticity = e > 1.0f ? 1.0f : e; }

			float GetElasticity() { return elasticity; }

			void SetFriction(float f) { friction = f > 1.0f ? 1.0f : f; }

			float GetFriction() { return friction; }

			void ApplyAngularImpulse(const Vector3& force);

			void ApplyLinearImpulse(const Vector3& force);
			
			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void PrintForce();

			void AddTorque(const Vector3& torque);

			void ClearForces();

			void SetCoeficient(float c) { coeficient = c; }
			float GetCoeficient() { return coeficient; }

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

		protected:
			const CollisionVolume* volume;
			Transform*		transform;
			
			//coefficient
			float coeficient = 0.33f;
			float inverseMass;
			float elasticity;
			float friction;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;
			
			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;
		};
	}
}

