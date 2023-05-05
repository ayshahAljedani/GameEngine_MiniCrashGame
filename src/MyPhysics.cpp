#include "PxPhysicsAPI.h"
#include "MyPhysics.h"

using namespace Aftr;
using namespace physx;

PhysXManager::PhysXManager() {
	this->Found = PxCreateFoundation(PX_PHYSICS_VERSION, this->Allocator, this->ErrorCallback);
	this->Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->Found, PxTolerancesScale());
	PxSceneDesc SC(this->Physics->getTolerancesScale());
	SC.filterShader = PxDefaultSimulationFilterShader;
	SC.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	this->Scene = this->Physics->createScene(SC);
	this->Scene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
}

PhysXManager* PhysXManager::Manager = nullptr;

PhysXManager* PhysXManager::New() {
	Manager = new PhysXManager;
	return Manager;
}

PhysXManager::~PhysXManager() {
	this->Scene->release();
	this->Physics->release();
	this->Found->release();
}