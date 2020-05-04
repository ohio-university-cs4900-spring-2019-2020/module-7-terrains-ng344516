#include "GLViewTerrain.h"

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
#include "AftrGLRendererBase.h"

//If we want to use way points, we need to include this.
#include "TerrainWayPoints.h"
#include "WOPhysXActor.h"
#include "WOPhysXTerrain.h"
#include "PhysicsModule.h"
#include "WOGridECEFElevation.h"
#include "io.h"
#include <iostream>
//#include <PxDefaultStreams.h>

using namespace Aftr;
using namespace physx;

GLViewTerrain* GLViewTerrain::New( const std::vector< std::string >& args )
{
   GLViewTerrain* glv = new GLViewTerrain( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewTerrain::GLViewTerrain( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewTerrain::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewTerrain::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewTerrain::onCreate()
{
   //GLViewTerrain::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   
   //this->physicsEngine = PhysicsModule::New(); // Instantiate physics engine

   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

   //PxMaterial* gMaterial = this->physicsEngine->getPhysics()->createMaterial(.1f, .1f, .6f);
   //PxMaterial* gMaterial2 = this->physicsEngine->getPhysics()->createMaterial(.1f, .1f, .6f);
   //PxRigidStatic* groundPlane = PxCreatePlane(*this->physicsEngine->getPhysics(), PxPlane(PxVec3(0, 0, 1), 0), *gMaterial);
   //this->physicsEngine->getScene()->addActor(*groundPlane); // Instantiate the ground

}


GLViewTerrain::~GLViewTerrain()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewTerrain::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
   this->physicsEngine->simulate(this->balls); // Simulate physics
}


void GLViewTerrain::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewTerrain::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewTerrain::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewTerrain::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewTerrain::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_1 )
   {

   }
   if (key.keysym.sym == SDLK_f)
   {
	   std::cout << "Spawning physics object\n";
	   spawnBall(spawnInFrontOfPlayer(10.0f));
   }
}


void GLViewTerrain::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewTerrain::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );

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

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );
   
   createTerrainWayPoints();
   this->physicsEngine = PhysicsModule::New();
   createGrid();
}


void GLViewTerrain::createTerrainWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = false;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
void GLViewTerrain::spawnBall(Vector spawnLoc) {
	PxTransform trans = PxTransform(PxVec3(spawnLoc.x, spawnLoc.y, spawnLoc.z));
	PxShape* shape = this->physicsEngine->physics->createShape(PxSphereGeometry(2.0f), *this->physicsEngine->physics->createMaterial(.5f, .3f, .45f));
	PxRigidDynamic* actor = PxCreateDynamic(*this->physicsEngine->physics, trans, *shape, 5.0f);
	std::cout << actor->getMass() << std::endl;
	WOPhysXActor* wo = WOPhysXActor::New(actor, "../mm/models/sphere.dae", Vector(1.0f, 1.0f, 1.0f));
	wo->getActor()->userData = wo;
	wo->setPosition(spawnLoc);
	worldLst->push_back(wo);
	std::cout << "At position " << wo->getPosition() << std::endl;
	this->physicsEngine->scene->addActor(*wo->getActor());
	wo->fire(this->cam->getLookDirection());

	this->balls.insert(std::pair(wo, numBalls));
	numBalls++;
}

Vector GLViewTerrain::spawnInFrontOfPlayer(float dist) {
	Vector position = this->cam->getPosition();
	Vector direction = this->cam->getLookDirection();
	return Vector(position.x + (dist * direction.x), position.y + (dist * direction.y), position.z + (dist * direction.z));
}

void GLViewTerrain::createGrid() {
	double top = 34.2072593790098f;
	double bottom = 33.9980272592999f;
	double left = -118.65234375f;
	double right = -118.443603515625f;
	double vert = top - bottom;
	double horz = right - left;

	VectorD offset((top + bottom) / 2.0, (left + right) / 2.0, 0.0);

	VectorD scale = VectorD(0.1f, 0.1f, 0.1f);
	VectorD upperLeft(top, left, 0);
	VectorD lowerRight(bottom, right, 0);
	WO* grid = WOPhysXTerrain::New(upperLeft, lowerRight, 0, offset, scale, "../mm/images/Woodland.tif");

	VectorD ll = ((upperLeft + lowerRight) / 2.0);
	ll.z = 0.0;
	VectorD zdir = ll.toECEFfromWGS84().normalizeMe();
	VectorD northPoleECEF = VectorD(90.0, 0.0, 0.0).toECEFfromWGS84();
	VectorD xdir = northPoleECEF - ll.toECEFfromWGS84();
	xdir = xdir.vectorProjectOnToPlane(zdir);
	xdir.normalize();
	VectorD ydir = zdir.crossProduct(xdir);
	ydir.normalize();
	//create the Local Tangent Plane (LTP) transformation matrix
	float localBodySpaceToLTP[16] = {
		(float)xdir.x, (float)xdir.y, (float)xdir.z, 0.0f,
		(float)ydir.x, (float)ydir.y, (float)ydir.z, 0.0f,
		(float)zdir.x, (float)zdir.y, (float)zdir.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	grid->getModel()->setDisplayMatrix(Mat4(localBodySpaceToLTP).transposeUpperLeft3x3());

	grid->setPosition(0.0f, 0.0f, -50.0f);
	grid->setLabel("grid");
	worldLst->push_back(grid);
	createTerrain(grid);

	// load and set texture to grid
	Texture* tex = ManagerTexture::loadTexture("../mm/images/Woodland2.bmp");
	for (size_t i = 0; i < grid->getModel()->getModelDataShared()->getModelMeshes().size(); i++)
		grid->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkin().getMultiTextureSet().at(0) = tex;
	grid->getModel()->isUsingBlending(false);
}

void GLViewTerrain::createTerrain(WO* wo) {
	size_t vertexListSize = wo->getModel()->getModelDataShared()->getCompositeVertexList().size();
	size_t indexListSize = wo->getModel()->getModelDataShared()->getCompositeIndexList().size();

	this->vertexListCopy = new float[vertexListSize * 3];
	this->indicesCopy = new unsigned int[indexListSize];

	for (size_t i = 0; i < vertexListSize; i++)
	{
		this->vertexListCopy[i * 3 + 0] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
		this->vertexListCopy[i * 3 + 1] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
		this->vertexListCopy[i * 3 + 2] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
	}
	for (size_t i = 0; i < indexListSize; i++)
		this->indicesCopy[i] = wo->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<uint32_t>(vertexListSize);
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = vertexListCopy;

	meshDesc.triangles.count = static_cast<uint32_t>(indexListSize) / 3;
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.data = this->indicesCopy;
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = this->physicsEngine->cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
	{
		std::cout << "Failed to create Triangular mesh" << std::endl;
		std::cin.get();
	}
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* mesh = this->physicsEngine->physics->createTriangleMesh(readBuffer);

	PxMaterial* gMaterial = this->physicsEngine->physics->createMaterial(0.5f, 0.5f, 0.6f);
	PxShape* shape = this->physicsEngine->physics->createShape(PxTriangleMeshGeometry(mesh), *gMaterial, true);
	PxTransform t({ 0, 0, 0 });

	Mat4 wo_pose = wo->getModel()->getDisplayMatrix();
	PxRigidStatic* actor = this->physicsEngine->physics->createRigidStatic(t);
	bool b = actor->attachShape(*shape);
	float p[] = { wo_pose[0], wo_pose[1], wo_pose[2], wo_pose[3], wo_pose[4], wo_pose[5], wo_pose[6], wo_pose[7], wo_pose[8], wo_pose[9], wo_pose[10], wo_pose[11], wo_pose[12], wo_pose[13], wo_pose[14], wo_pose[15] };
	Vector wo_position = wo->getPosition();
	p[12] = wo_position[0];
	p[13] = wo_position[1];
	p[14] = wo_position[2];
	actor->setGlobalPose(PxTransform(PxMat44(p)));

	this->physicsEngine->addActor(wo, actor);
}