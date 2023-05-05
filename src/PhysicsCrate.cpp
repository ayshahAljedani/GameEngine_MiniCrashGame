#include "PhysicsCrate.h"
#include "Model.h"
#include "ModelMeshSkin.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "PxPhysicsAPI.h"
#include "MyPhysics.h"

#include <string>
#include <vector>

using namespace Aftr;
using namespace physx;

PhysicsCrate::PhysicsCrate() : IFace(this), WO() {
	label = "Crate";
}

PhysicsCrate::~PhysicsCrate() {}

PhysicsCrate* PhysicsCrate::New() {
	PhysicsCrate* Crate = new PhysicsCrate();
	Crate->onCreate();
	Crate->initialize(Crate);
	ManagerGLView::getGLView()->getWorldContainer()->push_back(Crate);
	return Crate;
}

std::vector<PhysicsCrate*> PhysicsCrate::objList = {};

void PhysicsCrate::onCreate() {
	const std::string crate(ManagerEnvironmentConfiguration::getLMM() + "/models/NitroCrate/nitro.dae");
	WO::onCreate(crate, Vector(scale_x, scale_y, scale_z));
	PhysXManager* PhysicsManager = PhysXManager::Manager;
	PxMaterial* material = PhysicsManager->Physics->createMaterial(0.5f, 0.5f, 0.0f);
	PxShape* shape = PhysicsManager->Physics->createShape(PxBoxGeometry(scale_x * 2.51, scale_y * 2.51, scale_z * 2.51), *material, true);
	PxTransform t({ position[0], position[1], position[2] });
	actor = PhysicsManager->Physics->createRigidDynamic(t);
	actor->attachShape(*shape);
	actor->userData = this;
	actor->setName(label.c_str());
	PhysicsManager->Scene->addActor(*actor);
}





void PhysicsCrate::initialize(PhysicsCrate* wo) {
	wo->WO::setPosition(position[0], position[1], position[2]);
	wo->rotateAboutGlobalX(rotation[0]);
	wo->rotateAboutGlobalY(rotation[1]);
	wo->rotateAboutGlobalZ(rotation[2]);

	Mat4 m2 = this->getPose();
	PxMat44 m;
	for (int i = 0; i < 16; i++)
		m(i % 4, i / 4) = m2[i];

	PxTransform t(m);
	t.p = PxVec3(position[0], position[1], position[2]);
	this->actor->setGlobalPose(t);
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->upon_async_model_loaded([wo]() {
		ModelMeshSkin& skin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
		skin.getMultiTextureSet().at(0).setTexRepeats(1.0f);
		skin.setAmbient(aftrColor4f(0.7f, 0.7f, 0.7f, 1.0f)); //Color of object when it is not in any light
		skin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
		skin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
		skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
		});
	wo->setLabel(label);
}

void PhysicsCrate::updateObjectPose() {
	PxMat44 m(this->actor->getGlobalPose().q);
	Mat4 m2;
	for (int i = 0; i < 16; i++)
		m2[i] = m(i % 4, i / 4);

	this->setDisplayMatrix(m2);
	this->setPosition(this->actor->getGlobalPose().p.x, this->actor->getGlobalPose().p.y, this->actor->getGlobalPose().p.z);
}

void PhysicsCrate::setPosition(float x, float y, float z) {
	WO::setPosition(x, y, z);
	PxTransform t = this->actor->getGlobalPose();
	t.p = PxVec3(x, y, z);
	this->actor->setGlobalPose(t);
}
