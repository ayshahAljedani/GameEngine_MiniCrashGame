#pragma once

#include "WO.h"

#include <vector>

class Vector;
class string;

namespace physx {
	class PxRigidDynamic;
	class PxScene;
	class PxPhysics;
}

namespace Aftr {

	class NetMessengerClient;

	class PhysicsFruit: public WO {

		public:
			static PhysicsFruit* New();
			virtual ~PhysicsFruit();
			physx::PxRigidDynamic* actor = nullptr;
			virtual void updateObjectPose();
			virtual void setPosition(float x, float y, float z);
			static std::vector<PhysicsFruit*> objList;

		protected:
			PhysicsFruit();
			virtual void initialize(PhysicsFruit* wo);
			virtual void onCreate();
			float scale_radius = 1;
			std::string label;
			Vector position = Vector(0, 0, 0);
			Vector rotation = Vector(0, 0, 0);
	};
}
