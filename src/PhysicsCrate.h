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

	class PhysicsCrate : public WO {

	public:
		static PhysicsCrate* New();
		virtual ~PhysicsCrate();
		physx::PxRigidDynamic* actor = nullptr;
		virtual void updateObjectPose();
		virtual void setPosition(float x, float y, float z);
		static std::vector<PhysicsCrate*> objList;
		int respawn_time = -1;
		Vector position_initial = Vector(0, 0, 0);

	protected:
		PhysicsCrate();
		virtual void initialize(PhysicsCrate* wo);
		virtual void onCreate();
		float scale_x = 1, scale_y = 1, scale_z = 1;
		std::string label;
		Vector position = Vector(0, 0, 0);
		Vector rotation = Vector(0, 0, 0);
	};
}
