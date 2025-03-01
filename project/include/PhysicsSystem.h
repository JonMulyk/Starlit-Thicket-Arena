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

class ContactReportCallback : public physx::PxSimulationEventCallback {
public:
	int collisions = 0;
	std::vector<std::pair<physx::PxActor*, physx::PxActor*>> collisionPairs;

	void onContact(
		const physx::PxContactPairHeader& pairHeader,
		const physx::PxContactPair* pairs,
		physx::PxU32 nbPairs
	) override {
		std::cout << "Collision detected " << std::endl;
		collisions++;
		for (physx::PxU32 i = 0; i < nbPairs; i++) {
			const physx::PxContactPair& cp = pairs[i];
			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
				// Store colliding actors
				collisionPairs.push_back({ pairHeader.actors[0], pairHeader.actors[1] });
			}
		}
	}

	void clearCollisions() {
		collisionPairs.clear();
	}

	int getCollisions() {
		return collisions;
	}

	void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
	void onWake(physx::PxActor**, physx::PxU32) override {}
	void onSleep(physx::PxActor**, physx::PxU32) override {}
	void onTrigger(physx::PxTriggerPair*, physx::PxU32) override {}
	void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, physx::PxU32) override {}
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
	ContactReportCallback* gContactReportCallback = nullptr;

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

	void stepPhysics(float timestep, Command& command, Command& controllerCommand);

	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density=10.f);

	void addTrail(float x, float z, float rot);
	physx::PxVec3 getPos(int i);
	Transform* getTransformAt(int i);
	void updateTransforms(std::vector<Entity>& entityList);
	void updatePhysics(double dt);
};