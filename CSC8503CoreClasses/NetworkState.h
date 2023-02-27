#pragma once

namespace PhysicalProject {
	using namespace Maths;
	namespace GameDemo {
		//class GameObject;
		class NetworkState	{
		public:
			NetworkState();
			virtual ~NetworkState();

			Vector3		position;
			Quaternion	orientation;
			int			stateID;
		};
	}
}

