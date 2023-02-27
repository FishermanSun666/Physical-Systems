#pragma once

namespace PhysicalProject {
	namespace GameDemo {
		class Constraint	{
		public:
			Constraint() {}
			virtual ~Constraint() {}

			virtual void UpdateConstraint(float dt) = 0;
		};
	}
}