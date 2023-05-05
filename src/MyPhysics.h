#pragma once

#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"

namespace physx {
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
}

namespace Aftr {

	class PhysXManager {

	public:
		static PhysXManager* New();
		virtual ~PhysXManager();
		physx::PxFoundation* Found;
		physx::PxPhysics* Physics;
		physx::PxScene* Scene;
		physx::PxMaterial* PhysMaterial;
		physx::PxDefaultAllocator Allocator;
		physx::PxDefaultErrorCallback ErrorCallback;
		static PhysXManager* Manager;

	protected:
		PhysXManager();
	};
}
