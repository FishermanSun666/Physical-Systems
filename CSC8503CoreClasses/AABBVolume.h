#pragma once
#include "CollisionVolume.h"
#include "Vector3.h"

namespace PhysicalProject {
	using namespace PhysicalProject::Maths;
	class AABBVolume : CollisionVolume
	{
	public:
		AABBVolume(const Vector3& halfDims) {
			type		= VolumeType::AABB;
			halfSizes	= halfDims;
		}
		~AABBVolume() {

		}

		Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

	protected:
		Vector3 halfSizes;
	};
}
