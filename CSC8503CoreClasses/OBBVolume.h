#pragma once
#include "CollisionVolume.h"

namespace PhysicalProject {
	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes;
		}
	protected:
		Maths::Vector3 halfSizes;
	};
}

