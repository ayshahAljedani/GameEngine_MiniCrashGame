#pragma once

#include "WO.h"

class Vector;

namespace physx {
	class PxRigidStatic;
}

namespace Aftr {

	class PhysicsGrass: public WO {

		public:
			static PhysicsGrass* New();
			static PhysicsGrass* New(float x, float y, float z);
			virtual ~PhysicsGrass();
			physx::PxRigidStatic* actor = nullptr;

		protected:
			PhysicsGrass();
			PhysicsGrass(float x, float y, float z);
			virtual void initialize(PhysicsGrass* wo);
			virtual void onCreate();
			float pos_x = 0, pos_y = 0, pos_z = 0;
	};
}
