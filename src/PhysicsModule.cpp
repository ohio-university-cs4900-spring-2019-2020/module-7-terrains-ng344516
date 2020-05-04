#include "PhysicsModule.h"
#include "Model.h"

#include <iostream>

using namespace Aftr;
using namespace physx;

PhysicsModule::PhysicsModule() {
	this->foundation = PxCreateFoundation(version, allocator, error);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	this->pvd = PxCreatePvd(*foundation);
	this->pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	this->physics = PxCreatePhysics(version, *foundation, PxTolerancesScale(), true, pvd);
	this->cooking = PxCreateCooking(version, *foundation, PxCookingParams(PxTolerancesScale()));
	this->dispatch = PxDefaultCpuDispatcherCreate(4);
	this->material = physics->createMaterial(0.5f, 0.5f, 0.5f);

	PxSceneDesc description(this->physics->getTolerancesScale());
	description.gravity = PxVec3(0.0f, 0.0f, -9.81f);
	description.cpuDispatcher = this->dispatch;
	description.filterShader = PxDefaultSimulationFilterShader;
	description.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	this->scene = this->physics->createScene(description);
	this->client = scene->getScenePvdClient();

	if (this->client) {
		this->client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		this->client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		this->client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

PhysicsModule::~PhysicsModule() {
	this->physics->release();
	this->scene->release();
	this->foundation->release();
} 

 PhysicsModule* PhysicsModule::New() {
	return new PhysicsModule();
}

void PhysicsModule::simulate(std::map<WOPhysXActor*, int> balls) {
	this->scene->simulate(ManagerSDLTime::getTimeSinceLastPhysicsIteration() / 1000.0f);
	this->scene->fetchResults(true);
	PxU32 numActors = 0;
	PxActor** activeActors = this->scene->getActiveActors(numActors);
	for (PxU32 i = 0; i < numActors; i++) {
		WOPhysXActor* wo = static_cast<WOPhysXActor*>(activeActors[i]->userData);
		if (wo != nullptr && wo->getActor() != nullptr) {
			PxTransform trans = wo->getActor()->getGlobalPose();
			PxMat44 pose = PxMat44(trans);

			float convert[16] = {
				pose(0, 0), pose(0, 1), pose(0, 2), pose(3, 0),
				pose(1, 0), pose(1, 1), pose(1, 2), pose(3, 1),
				pose(2, 0), pose(2, 1), pose(2, 2), pose(3, 2),
				pose(0, 3), pose(1, 3), pose(2, 3), pose(3, 3)
			};
			Mat4 mat(convert);
			wo->setDisplayMatrix(mat);
			wo->setPosition(Vector(mat[12], mat[13], mat[14]));
		}
	}
}


physx::PxPhysics* PhysicsModule::getPhysics() {
	return this->physics;
}
physx::PxScene* PhysicsModule::getScene() {
	return this->scene;
}
physx::PxFoundation* PhysicsModule::getFoundation() {
	return this->foundation;
}

physx::PxCooking* PhysicsModule::getCooking() {
	return this->cooking;
}

void PhysicsModule::addActor(void* pointer, PxActor* actor) {
	actor->userData = pointer;
	this->scene->addActor(*actor);
}
