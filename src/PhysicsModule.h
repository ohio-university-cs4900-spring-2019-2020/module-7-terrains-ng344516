#pragma once

#include "PxPhysicsAPI.h"
#include <map>
#include "WOPhysXActor.h"

namespace Aftr {

	class ModelDataSharedID;
	class WOPhysXActor;
	
	class PhysicsModule {
	public:
		PhysicsModule();
		~PhysicsModule();
		static PhysicsModule* New();
		void simulate(std::map<WOPhysXActor*, int> balls);
		physx::PxPhysics* getPhysics();
		physx::PxScene* getScene();
		physx::PxFoundation* getFoundation();
		physx::PxCooking* getCooking();
		physx::PxPhysics* physics;
		physx::PxScene* scene;
		physx::PxFoundation* foundation;
		physx::PxCooking* cooking;

		void addActor(void* pointer, physx::PxActor* actor);


	protected:
		
		physx::PxPvd* pvd;
		physx::PxU32 version = PX_PHYSICS_VERSION;
		
		physx::PxDefaultCpuDispatcher* dispatch;
		physx::PxMaterial* material;
		physx::PxDefaultAllocator allocator;
		physx::PxDefaultErrorCallback error;
		physx::PxPvdSceneClient* client;

		std::map<ModelDataSharedID, physx::PxShape*> spheres;

		
	};

} // Namespace Aftr