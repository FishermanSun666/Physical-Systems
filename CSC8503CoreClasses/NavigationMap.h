#pragma once
#include "Vector3.h"
#include "NavigationPath.h"
namespace PhysicalProject {
	using namespace PhysicalProject::Maths;
	namespace GameDemo {
		class NavigationMap
		{
		public:
			NavigationMap() {}
			~NavigationMap() {}

			virtual bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) = 0;
		};
	}
}

