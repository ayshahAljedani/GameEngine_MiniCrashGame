#include "GLViewAyshahFinal.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"

#include "PxPhysicsAPI.h"
#include "irrKlang.h"
#include "MyPhysics.h"
#include "PhysicsGrass.h"
#include "PhysicsCrate.h"
#include "PhysicsFruit.h"


using namespace Aftr;
using namespace physx;
GLViewAyshahFinal* GLViewAyshahFinal::New( const std::vector< std::string >& args )
{
   GLViewAyshahFinal* glv = new GLViewAyshahFinal( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewAyshahFinal::GLViewAyshahFinal( const std::vector< std::string >& args ) : GLView( args )
{
    engine = createIrrKlangDevice();
    soundMusic = engine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/music.ogg").c_str());
    soundCrystal = engine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/crystal.ogg").c_str());
    soundSpinning = engine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/crash-spin.ogg").c_str());
    soundFruit = engine->addSoundSourceFromFile((ManagerEnvironmentConfiguration::getLMM() + "/sounds/fruit.ogg").c_str());

    soundCrystal->setDefaultMinDistance(60.0f);
    soundSpinning->setDefaultMinDistance(60.0f);
    soundFruit->setDefaultMinDistance(60.0f);
}


void GLViewAyshahFinal::onCreate()
{
   //GLViewAyshahFinal::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewAyshahFinal::~GLViewAyshahFinal()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewAyshahFinal::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
   if (IsMoving) {

       engine->setListenerPosition(
           vec3df(this->cam->getPosition().x, this->cam->getPosition().y, this->cam->getPosition().z),
           vec3df(this->cam->getCameraLookAtPoint().x, this->cam->getCameraLookAtPoint().y, this->cam->getCameraLookAtPoint().z),
           vec3df(0, 0, 0),
           vec3df(this->cam->getNormalDirection().x, this->cam->getNormalDirection().y, this->cam->getNormalDirection().z)
       );


       if (Crash) {
           Crash->setPosition(Crash_posx, Crash_posy, Crash_posz);
           Crash->rotateAboutGlobalX(Crash_roglobx);
           Crash->rotateAboutGlobalY(Crash_rogloby);
           Crash->rotateAboutGlobalZ(Crash_roglobz);
           Crash->rotateAboutRelX(Crash_rorelx);
           Crash->rotateAboutRelY(Crash_rorely);
           Crash->rotateAboutRelZ(Crash_rorelz);
       }


       PhysicsManager->Scene->setGravity(PxVec3(gravity_x, gravity_y, gravity_z));
       PhysicsManager->Scene->simulate(0.01f);
       PxU32 errorState = 0;
       PhysicsManager->Scene->fetchResults(true);
       {
           PxU32 numActors = 0;
           PxActor** actors = PhysicsManager->Scene->getActiveActors(numActors);
           for (PxU32 i = 0; i < numActors; i++) {
               PxActor* actor = actors[i];
               if (auto ptr = static_cast<PhysicsCrate*>(actor->userData))
                   ptr->updateObjectPose();
               else if (auto ptr = static_cast<PhysicsFruit*>(actor->userData))
                   ptr->updateObjectPose();
           }
       }
       checkCollision();
   }
   else {
       engine->stopAllSounds();
   }
}


void GLViewAyshahFinal::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewAyshahFinal::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewAyshahFinal::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewAyshahFinal::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewAyshahFinal::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if (key.keysym.sym == SDLK_SPACE)
   {
       Crash_rorelz = 1;
       engine->play3D(soundSpinning, vec3df(Crash->getPosition().x, Crash->getPosition().y, Crash->getPosition().z));
   }

   if (key.keysym.sym == SDLK_d)
   {
       Crash_posx += 1;
   }

   if (key.keysym.sym == SDLK_a)
   {
       Crash_posx -= 1;
   }

   if (key.keysym.sym == SDLK_w)
   {
       Crash_posy -= 1;
   }
   if (key.keysym.sym == SDLK_s)
   {
       Crash_posy += 1;
   }
}


void GLViewAyshahFinal::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );

   if (key.keysym.sym == SDLK_SPACE)
   {
       Crash_rorelz = 0;
   }
}


void Aftr::GLViewAyshahFinal::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = false;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );
   PhysicsManager = PhysXManager::New();

   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   std::string crash(ManagerEnvironmentConfiguration::getLMM() + "/models/Crash/crash.obj");
   std::string music(ManagerEnvironmentConfiguration::getLMM() + "/sounds/music.ogg");
   std::string crystal(ManagerEnvironmentConfiguration::getLMM() + "/models/Crystal/crystal.dae");
   std::string fruit(ManagerEnvironmentConfiguration::getLMM() + "/models/WumpaFruit/wumpa_fruit.dae");
   std::string crate(ManagerEnvironmentConfiguration::getLMM() + "/models/Crate/crate.dae");
   engine->play2D(soundMusic, 1);

   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   PhysicsGrass::New(0, 0, -25);

   for (int i = 0; i < 15; i++) {
       PhysicsCrate* Crate = PhysicsCrate::New();
       Crate->setPosition((rand() % 200) - (rand() % 400), (rand() % 200) - (rand() % 400), (rand() % 200));
       Crate->position_initial = Vector(Crate->getPosition());
       Crate->actor->addForce(PxVec3((rand() % 3000) - (rand() % 6000), (rand() % 3000) - (rand() % 6000), (rand() % 3000) - (rand() % 6000)));
       crateIDs.push_back(Crate->getID());
   }

   for (int i = 0; i < 30; i++) {
       PhysicsFruit* Fruit = PhysicsFruit::New();
       Fruit->setPosition((rand() % 600) - 300, (rand() % 600) - 300, (rand() % 200));
       Fruit->actor->addForce(PxVec3((rand() % 3000) - (rand() % 6000), (rand() % 3000) - (rand() % 6000), (rand() % 3000) - (rand() % 6000)));
       fruitIDs.push_back(Fruit->getID());
   }


   {
      //Create a light
     float ga = 0.1f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, 100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }

   {
      //Create the SkyBox
      WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->setLabel( "Sky Box" );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      worldLst->push_back( wo );
   }

  

   {
       ////Create the carash opject
       Crash = WO::New(crash, Vector(0.3, 0.3, 0.3), MESH_SHADING_TYPE::mstFLAT);
       Crash->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Crash->setLabel("Crash");
       worldLst->push_back(Crash);
   }

   for (int i = 0; i < 15; i++) {
       WO* Crystal = WO::New(crystal, Vector(0.09, 0.09, 0.09), MESH_SHADING_TYPE::mstFLAT);
       Crystal->setPosition(Vector((rand() % 600) - 300, (rand() % 600) - 300, -18));
       Crystal->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Crystal->setLabel("Crystal");
       worldLst->push_back(Crystal);
       crystalIDs.push_back(Crystal->getID());
   }

   for (int i = 0; i < 15; i++) {
       WO* Fruit = WO::New(fruit, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
       Fruit->setPosition(-50 + i * 10, 20, -18);
       Fruit->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Fruit->setLabel("Fruit");
       worldLst->push_back(Fruit);
       fruitIDs.push_back(Fruit->getID());
   }

   for (int i = 0; i < 15; i++) {
       WO* Fruit = WO::New(fruit, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
       Fruit->setPosition(-250, 12 + i * 10, -18);
       Fruit->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Fruit->setLabel("Fruit");
       worldLst->push_back(Fruit);
       fruitIDs.push_back(Fruit->getID());
   }

   for (int i = 0; i < 15; i++) {
       WO* Fruit = WO::New(fruit, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
       Fruit->setPosition(-200, -10 + i * 10, -18);
       Fruit->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Fruit->setLabel("Fruit");
       worldLst->push_back(Fruit);
       fruitIDs.push_back(Fruit->getID());
   }

   for (int i = 0; i < 15; i++) {
       WO* Crate = WO::New(crate, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
       Crate->setPosition(Vector((rand() % 600) - 300, (rand() % 600) - 300, -18));
       Crate->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       Crate->setLabel("Crate");
       worldLst->push_back(Crate);
       crateIDs.push_back(Crate->getID());
   }
   
   //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
   //the default Dear ImGui demo that shows all the features... To create your own,
   //inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
   WOImGui* gui = WOImGui::New( nullptr );
   gui->setLabel( "My Gui" );
   gui->subscribe_drawImGuiWidget(
      [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
      {
           
      } );
   this->worldLst->push_back( gui );

   createAyshahFinalWayPoints();
}


void GLViewAyshahFinal::createAyshahFinalWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = false;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}

void GLViewAyshahFinal::checkCollision() {
    auto crashPos = Crash->getPosition();

    for (int i = 0; i < crateIDs.size(); i++) {
        WO* crateWO = worldLst->getWOByID(crateIDs[i]);

        if (auto crate = dynamic_cast<PhysicsCrate*>(crateWO)) {
            Vector cratePos = crate->getPosition();
            if (crashPos[0] > cratePos[0] - 5 && crashPos[0] < cratePos[0] + 5 &&
                crashPos[1] > cratePos[1] - 5 && crashPos[1] < cratePos[1] + 5 &&
                crashPos[2] > cratePos[2] - 5 && crashPos[2] < cratePos[2] + 5) {
                IsMoving = false;
            }

            if (crate->getPosition().z < -15) {
                if (crate->respawn_time == -1)
                    crate->respawn_time = 90;
                else if (crate->respawn_time > 0)
                    --crate->respawn_time;
                else {
                    crate->respawn_time = -1;
                    crate->actor->setLinearVelocity(PxVec3(0, 0, 0));
                    crate->setPosition(crate->position_initial.x, crate->position_initial.y, crate->position_initial.z);
                }
            }
        }
        else {
            Vector cratePos = crateWO->getPosition();
            if ((crashPos[0] > cratePos[0] - 5 && crashPos[0] < cratePos[0] + 5 &&
                crashPos[1] > cratePos[1] - 5 && crashPos[1] < cratePos[1] + 5 &&
                crashPos[2] > cratePos[2] - 5 && crashPos[2] < cratePos[2] + 5) &&
                Crash_rorelz == 1) {
                vec3df soundpos = vec3df(cratePos.x, cratePos.y, cratePos.z);
                crateWO->setPosition(0, 0, -1000);
            }
        }
    }

    for (int i = 0; i < fruitIDs.size(); i++) {
        WO* fruitWO = worldLst->getWOByID(fruitIDs[i]);

        if (auto fruit = dynamic_cast<PhysicsFruit*>(fruitWO)) {
            Vector fruitPos = fruit->getPosition();
            if (crashPos[0] > fruitPos[0] - 5 && crashPos[0] < fruitPos[0] + 5 &&
                crashPos[1] > fruitPos[1] - 5 && crashPos[1] < fruitPos[1] + 5 &&
                crashPos[2] > fruitPos[2] - 10 && crashPos[2] < fruitPos[2] + 10) {
                vec3df soundpos = vec3df(fruitPos.x, fruitPos.y, fruitPos.z);
                fruit->setPosition(0, 0, -1000);
                engine->play3D(soundFruit, soundpos);
            }
        }
        else {
            Vector fruitPos = fruitWO->getPosition();
            if (crashPos[0] > fruitPos[0] - 5 && crashPos[0] < fruitPos[0] + 5 &&
                crashPos[1] > fruitPos[1] - 5 && crashPos[1] < fruitPos[1] + 5 &&
                crashPos[2] > fruitPos[2] - 10 && crashPos[2] < fruitPos[2] + 10) {
                vec3df soundpos = vec3df(fruitPos.x, fruitPos.y, fruitPos.z);
                fruitWO->setPosition(0, 0, -1000);
                engine->play3D(soundFruit, soundpos);
            }
        }
    }

    for (int i = 0; i < crystalIDs.size(); i++) {
        WO* crystalWO = worldLst->getWOByID(crystalIDs[i]);

        Vector crystalPos = crystalWO->getPosition();
        if (crashPos[0] > crystalPos[0] - 5 && crashPos[0] < crystalPos[0] + 5 &&
            crashPos[1] > crystalPos[1] - 5 && crashPos[1] < crystalPos[1] + 5 &&
            crashPos[2] > crystalPos[2] - 10 && crashPos[2] < crystalPos[2] + 10) {
            vec3df soundpos = vec3df(crystalPos.x, crystalPos.y, crystalPos.z);
            crystalWO->setPosition(0, 0, -1000);
            engine->play3D(soundCrystal, soundpos);
        }
    }
}
