#pragma once

#include "GLView.h"
#include "PhysicsModule.h"

namespace Aftr
{
   class Camera;
   class PhysicsModule;

/**
   \class GLViewTerrain
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewTerrain : public GLView
{
public:
   static GLViewTerrain* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewTerrain();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createTerrainWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   void createGrid();
   void createTerrain(WO* wo);

   void spawnBall(Vector spawnLoc);
   Vector spawnInFrontOfPlayer(float dist);
   std::map<WOPhysXActor*, int> balls;
   int numBalls = 0;

protected:
   GLViewTerrain( const std::vector< std::string >& args );
   virtual void onCreate();
   PhysicsModule* physicsEngine;
   float* vertexListCopy;
   unsigned int* indicesCopy;
};

/** \} */

} //namespace Aftr
