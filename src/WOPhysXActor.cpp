#include "WOPhysXActor.h"

#include <iostream>

using namespace Aftr;
using namespace physx;

WOPhysXActor* WOPhysXActor::New(physx::PxRigidDynamic* pxActor,
	const std::string& path,
	Vector scale,
	MESH_SHADING_TYPE shadingType)
{
	WOPhysXActor* wo = new WOPhysXActor(pxActor);
	wo->onCreate(path, scale, shadingType);
	return wo;
}

WOPhysXActor::WOPhysXActor(physx::PxRigidDynamic* pxActor) : IFace(this), WO() {
	this->physxActor = pxActor;
	this->live = true;
	this->stop = 1;
}

void WOPhysXActor::onCreate(const std::string& path,
	Vector scale,
	MESH_SHADING_TYPE shadingType) {
	WO::onCreate(path, scale, shadingType);
}


void WOPhysXActor::setDisplayMatrix(Mat4 matrix) {
	WO::getModel()->setDisplayMatrix(matrix);
}

physx::PxRigidDynamic* WOPhysXActor::getActor() {
	return physxActor;
}

void WOPhysXActor::fire(Vector direction) {
	PxVec3 pvec_dir = PxVec3(direction.x * 500000.0f, direction.y * 500000.0f, direction.z * 500000.0f);
	std::cout << "Fire force: (" << pvec_dir.x << ", " << pvec_dir.y << ", " << pvec_dir.z << ")\n";
	this->physxActor->addForce(pvec_dir);
	this->forceDirection = direction * (-1.0f, -1.0f, -1.0f);

}

void WOPhysXActor::update(int i) {
	if (i == stop) {
		std::cout << "Stop applied";
		PxVec3 pvec_dir = PxVec3(this->forceDirection.x * 300.0f, this->forceDirection.y * 300.0f, this->forceDirection.z * 300.0f);
		this->physxActor->addForce(pvec_dir);
		stop -= i;
	}
}
