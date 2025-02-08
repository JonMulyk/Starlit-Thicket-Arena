#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

#include "Transform.h"
#include "Entity.h"
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

		std::cout << "Stop colliding with me !" << std::endl;
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
	physx::PxU32 gear;
	physx::PxF32 duration;
};

class PhysicsSystem {
private:
	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;

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
	snippetvehicle2::EngineDriveVehicle gVehicle;
	snippetvehicle2::PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	const physx::PxU32 gTargetGearCommand = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	// Scripted Events in steps
	Command gCommands[5] = {
		{0.5f, 0.0f, 0.0f, gTargetGearCommand, 2.0f},	//brake on and come to rest for 2 seconds
		{0.0f, 0.65f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
		{0.5f, 0.0f, 0.0f, gTargetGearCommand, 5.0f},	//brake for 5 seconds
		{0.0f, 0.75f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
		{0.0f, 0.25f, 0.5f, gTargetGearCommand, 5.0f}	//light throttle and steer for 5 seconds.
	};

	const physx::PxU32 gNbCommands = sizeof(gCommands) / sizeof(Command);
	physx::PxReal gCommandTime = 0.0f;			//Time spent on current command
	physx::PxU32 gCommandProgress = 0;			//The id of the current command.

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
	PhysicsSystem();

	~PhysicsSystem();

	void stepPhysics();

	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density=10.f);

	physx::PxVec3 getPos(int i);
	Transform* getTransformAt(int i);
	void updateTransforms(std::vector<Entity>& entityList);
	void updatePhysics(double dt, std::vector<Entity> entityList);
};