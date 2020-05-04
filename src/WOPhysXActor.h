#pragma once

#include "WO.h"
#include "Model.h"
#include "PhysicsModule.h"
#include "PxPhysicsAPI.h"

namespace Aftr {
	class NetMessengerClient;
	class PhysicsModule;

	class WOPhysXActor : public WO {
	public:
		WOMacroDeclaration(WOPhysXActor, WO);
		static WOPhysXActor* New(physx::PxRigidDynamic* pxActor = nullptr,
			const std::string& path = "../mm/models/sphere.dae",
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);
		virtual void onCreate(const std::string& path,
			Vector scale = Vector(1, 1, 1),
			MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO);
		void setDisplayMatrix(Mat4 matrix);
		physx::PxRigidDynamic* getActor();
		void fire(Vector direction);
		void update(int i);

		int index;
	protected:
		WOPhysXActor(physx::PxRigidDynamic* pxActor);
		physx::PxRigidDynamic* physxActor;
		Vector forceDirection;
		bool live;
		int stop;
	};

} // Namespace Aftr