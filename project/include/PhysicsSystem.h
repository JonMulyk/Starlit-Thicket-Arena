#pragma once

#include "PxPhysicsAPI.h"
#include "Transform.h"
#include "Entity.h"
#include <vector>
#include <iostream>

struct MaterialProp {
	physx::PxReal staticFriction;
	physx::PxReal dynamicFriction;
	physx::PxReal restitution;
};

class PhysicsSystem {
private:
	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;

	//PhysX management class instances.
	physx::PxDefaultAllocator gAllocator;
	physx::PxDefaultErrorCallback gErrorCallback;
	physx::PxFoundation* gFoundation = NULL;
	physx::PxPhysics* gPhysics = NULL;
	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene* gScene = NULL;
	physx::PxPvd* gPvd = NULL;

public:
	PhysicsSystem();

	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density=10.f);

	physx::PxVec3 getPos(int i);
	Transform* getTransformAt(int i);
	void updateTransforms(std::vector<Entity>& entityList);
	void updatePhysics(double dt, std::vector<Entity> entityList);
};