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

struct VehicleData {
	snippetvehicle2::EngineDriveVehicle vehicle; // The PhysX vehicle instance
	physx::PxVec3 prevPos;                       // Previous position for trail calculation
	physx::PxVec3 prevDir;                       // Previous direction for trail calculation
	std::string name;                            // Unique name for the vehicle actor
	bool alive;
};

class ContactReportCallback : public snippetvehicle2::PxSimulationEventCallback {
	std::pair<physx::PxActor*, physx::PxActor*> collisionPair;
	bool newCollision = false;

	void onContact(
		const snippetvehicle2::PxContactPairHeader& pairHeader,
		const snippetvehicle2::PxContactPair* pairs,
		physx::PxU32 nbPairs
	) {
		PX_UNUSED(pairHeader);
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);

		newCollision = true;
		collisionPair = { pairHeader.actors[0], pairHeader.actors[1] };
	}
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
		const physx::PxTransform* poseBuffer,
		const physx::PxU32 count) {}

	public:
		std::pair<physx::PxActor*, physx::PxActor*> getCollisionPair() {
			return collisionPair;
		}

		bool checkCollision() {
			return newCollision;
		}

		void readNewCollision() {
			newCollision = false;
		}

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

	//snippetvehicle2::EngineDriveVehicle gVehicle;

	//physx::PxVec3 vehiclePrevPos;
	//physx::PxVec3 vehiclePrevDir;

	std::vector<VehicleData> vehicles;  
	std::vector<Command> vehicleCommands; 

	Model* trailModel;
	float trailStep = 2.f;

	void initPhysX();
	void cleanupPhysX();
	void initGroundPlane();
	void initBoarder();
	void cleanupGroundPlane();
	void initMaterialFrictionTable();
	bool initVehicles();
	void cleanupVehicles();
	bool initPhysics();
	void cleanupPhysics();

public:
	PhysicsSystem(GameState& gameState, Model* tModel);

	~PhysicsSystem();

	void stepPhysics(float timestep, Command& command, Command& controllerCommand);

	void setVehicleCommand(size_t vehicleIndex, const Command& cmd);

	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density=10.f);

	void addTrail(float x, float z, float rot, const char* name);
	physx::PxVec3 getPos(int i);
	Transform* getTransformAt(int i);
	void updateTransforms(std::vector<Entity>& entityList);
	void updatePhysics(double dt);
};