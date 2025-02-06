#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem() {
	// Initialize PhysX
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation) {
		throw std::exception("PxCreateFoundation failed!"); // TODO: custom exception
	}

	// PVD
	gPvd = PxCreatePvd(*gFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// Physics
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
	if (!gPhysics) {
		throw std::exception("PxCreatePhysics failed!");
	}

	// Scene
	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	// Prep PVD
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// Simulate
	physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 50), *material);
	gScene->addActor(*groundPlane);
}

void PhysicsSystem::addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density) {
	physx::PxMaterial*pMaterial = gPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.restitution);

	// Define a box
	physx::PxShape* shape = gPhysics->createShape(*geom, *pMaterial);
	physx::PxTransform tran(physx::PxVec3(0));
	physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(tran.transform(transform));

	// Add to lists
	rigidDynamicList.push_back(body);
	transformList.push_back(new Transform());

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, density);
	gScene->addActor(*body);

	// Clean up
	shape->release();
	pMaterial = nullptr;
}

physx::PxVec3 PhysicsSystem::getPos(int i) {
	physx::PxVec3 position = rigidDynamicList[i]->getGlobalPose().p;
	return position;
}

Transform* PhysicsSystem::getTransformAt(int i) {
	return transformList[i];
}

void PhysicsSystem::updateTransforms(std::vector<Entity>& entityList)
{
	for (int i = 0; i < entityList.size(); i++) {
		entityList.at(i).transform->pos.x = rigidDynamicList[i]->getGlobalPose().p.x;
		entityList.at(i).transform->pos.y = rigidDynamicList[i]->getGlobalPose().p.y;
		entityList.at(i).transform->pos.z = rigidDynamicList[i]->getGlobalPose().p.z;

		entityList.at(i).transform->rot.x = rigidDynamicList[i]->getGlobalPose().q.x;
		entityList.at(i).transform->rot.y = rigidDynamicList[i]->getGlobalPose().q.y;
		entityList.at(i).transform->rot.z = rigidDynamicList[i]->getGlobalPose().q.z;
		entityList.at(i).transform->rot.w = rigidDynamicList[i]->getGlobalPose().q.w;
	}
}

void PhysicsSystem::updatePhysics(double dt, std::vector<Entity> entityList) {
	gScene->simulate(dt);
	gScene->fetchResults(true);

	updateTransforms(entityList);
}