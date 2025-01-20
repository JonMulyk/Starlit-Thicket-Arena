#pragma once

#include "PxPhysicsAPI.h"
#include <vector>
#include <iostream>

class PhysicsSystem
{
	public:
		std::vector<physx::PxRigidDynamic*> rigidDynamicList;

		//PhysX management class instances.
		physx::PxDefaultAllocator gAllocator;
		physx::PxDefaultErrorCallback gErrorCallback;
		physx::PxFoundation* gFoundation = NULL;
		physx::PxPhysics* gPhysics = NULL;
		physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
		physx::PxScene* gScene = NULL;
		physx::PxMaterial* gMaterial = NULL;
		physx::PxPvd* gPvd = NULL;

		PhysicsSystem();

		physx::PxVec3 getPos(int i);


};