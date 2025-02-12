#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

#include "Transform.h"
#include "Entity.h"
#include "GameState.h"
#include <vector>
#include <iostream>

class ContactReportCallback : public snippetvehicle2::PxSimulationEventCallback {
	void onContact(
		const snippetvehicle2::PxContactPairHeader& pairHeader,
		const snippetvehicle2::PxContactPair* pairs,
		physx::PxU32 nbPairs
	) {
		PX_UNUSED(pairHeader);
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);
	}
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
		const physx::PxTransform* poseBuffer,
		const physx::PxU32 count) {}
};

struct MaterialProp {
	physx::PxReal staticFriction;
	physx::PxReal dynamicFriction;
	physx::PxReal restitution;
};

struct Command {
	physx::PxF32 brake;
	physx::PxF32 throttle;
	physx::PxF32 steer;
};

class PhysicsSystem {
private:
	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;
	GameState& gState;

	//PhysX management class instances.
	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxDefaultErrorCallback gErrorCallback;
	physx::PxDefaultAllocator gAllocator;
	physx::PxFoundation* gFoundation = NULL;
	physx::PxMaterial* gMaterial = NULL;
	physx::PxPhysics* gPhysics = NULL;
	physx::PxScene* gScene = NULL;
	physx::PxPvd* gPvd = NULL;

	// Material properties
	snippetvehicle2::PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	physx::PxU32 gNbPhysXMaterialFrictions = 0;
	physx::PxReal gPhysXDefaultMaterialFriction = 1.0f;

	// physical properties
	physx::PxVec3 gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);


	// Ground plane
	physx::PxRigidStatic* gGroundPlane = NULL;

	// Vehicles
	const char* gVehicleDataPath = "project/assets/vehicleData";
	const char* gVehicleName = "engineDrive";
	snippetvehicle2::PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	snippetvehicle2::EngineDriveVehicle gVehicle;
	physx::PxVec3 vehiclePrevPos;
	physx::PxVec3 vehiclePrevDir;
	Model& trailModel;
	float trailStep = 2.f;

	void initPhysX();
	void cleanupPhysX();
	void initGroundPlane();
	void cleanupGroundPlane();
	void initMaterialFrictionTable();
	bool initVehicles();
	void cleanupVehicles();
	bool initPhysics();
	void cleanupPhysics();

public:
	PhysicsSystem(GameState& gameState, Model& tModel);

	~PhysicsSystem();

	void stepPhysics(float timestep, Command& command);

	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density=10.f);

	void addTrail(float x, float z, float rot);
	physx::PxVec3 getPos(int i);
	Transform* getTransformAt(int i);
	void updateTransforms(std::vector<Entity>& entityList);
	void updatePhysics(double dt);
};