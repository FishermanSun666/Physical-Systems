#pragma once
#include "Vector3.h"
#include "NavigationPath.h"
namespace NCL {
	using namespace NCL::Maths;
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

