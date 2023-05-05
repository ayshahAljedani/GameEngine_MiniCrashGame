#pragma once

#include "GLView.h"
#include "GLView.h"
#include "irrklang.h"
#include "PxPhysicsAPI.h"
#include "extensions\PxDefaultErrorCallback.h"
#include "extensions\PxDefaultAllocator.h"
#include "PhysicsCrate.h"
#include "MyPhysics.h"
#include "PhysicsFruit.h"

using namespace irrklang;

namespace Aftr
{
   class Camera;
   class Camera;
   class irrklang;
   class PhysXManager;

/**
   \class GLViewAyshahFinal
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewAyshahFinal : public GLView
{
public:
   static GLViewAyshahFinal* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewAyshahFinal();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createAyshahFinalWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void checkCollision();
   bool IsMoving = true;
   float gravity_x = 0, gravity_y = 0, gravity_z = -50.0f;
protected:
   GLViewAyshahFinal( const std::vector< std::string >& args );
   virtual void onCreate();

private:
    WO* Crash = nullptr;
    WO* Crystal = nullptr;
    WO* Crates = nullptr;

    float Crash_posx = 0, Crash_posy = 0, Crash_posz = -18.0f;
    float Crash_roglobx = 0, Crash_rogloby = 0, Crash_roglobz = 0;
    float Crash_rorelx = 0, Crash_rorely = 0, Crash_rorelz = 0;

    PhysXManager* PhysicsManager = nullptr;
    ISoundEngine* engine = nullptr;

    ISoundSource* soundMusic = nullptr;
    ISoundSource* soundCrate = nullptr;
    ISoundSource* soundSpinning = nullptr;
    ISoundSource* soundFruit = nullptr;
    ISoundSource* soundCrystal = nullptr;

    std::vector<int> crateIDs;
    std::vector<int> fruitIDs;
    std::vector<int> crystalIDs;
};

/** \} */

} //namespace Aftr
