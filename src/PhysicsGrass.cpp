#include "PhysicsGrass.h"
#include "Model.h"
#include "ModelMeshSkin.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "PxPhysicsAPI.h"
#include "PhysXManager.h"

#include <string>
#include <vector>

class Vector;

using namespace std;
using namespace Aftr;
using namespace physx;

PhysicsGrass::PhysicsGrass(): IFace(this), WO() {}

PhysicsGrass::PhysicsGrass(float x, float y, float z): IFace(this), WO() {
	pos_x = x;
	pos_y = y;
	pos_z = z;
}

PhysicsGrass::~PhysicsGrass() {}

PhysicsGrass* PhysicsGrass::New() {
	PhysicsGrass* GrassPlane = new PhysicsGrass();
	GrassPlane->onCreate();
	GrassPlane->initialize(GrassPlane);
	ManagerGLView::getGLView()->getWorldContainer()->push_back(GrassPlane);
	return GrassPlane;
}

PhysicsGrass* PhysicsGrass::New(float x, float y, float z) {
	PhysicsGrass* GrassPlane = new PhysicsGrass(x, y, z);
	GrassPlane->onCreate();
	GrassPlane->initialize(GrassPlane);
	ManagerGLView::getGLView()->getWorldContainer()->push_back(GrassPlane);
	return GrassPlane;
}

void PhysicsGrass::onCreate() {
	const string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	WO::onCreate(grass, Vector(3, 3, 3), MESH_SHADING_TYPE::mstFLAT);
	PhysXManager* PhysicsManager = PhysXManager::Manager;
	PxMaterial* material = PhysicsManager->Physics->createMaterial(0.6f, 0.6f, 0.0f);
	actor = PxCreatePlane(*PhysicsManager->Physics, PxPlane(0, 0, 1, -pos_z), *material);
	PhysicsManager->PhysicsScene->addActor(*actor);
}

void PhysicsGrass::initialize(PhysicsGrass* wo) {
	wo->setPosition(Vector(pos_x, pos_y, pos_z));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	wo->upon_async_model_loaded([wo]() {
		ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
		grassSkin.getMultiTextureSet().at(0).setTexRepeats(5.0f);
		grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
		grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
		grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
		grassSkin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
	});
	wo->setLabel("Grass");
}
