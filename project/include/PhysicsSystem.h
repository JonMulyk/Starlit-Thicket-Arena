#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "Transform.h"
#include "Model.h"
#include "Entity.h"
#include "GameState.h"
#include "Command.h"
#include "Vehicle.h"

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

class PhysicsSystem {
private:
	// Helpers to track state
	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;
	std::vector<Model> pModels;
	Model* carModel;
	std::vector<Model> modelList;
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
	float trailStep = 2.f;

	//Trail lifetime management
	struct TrailSegment {
		physx::PxRigidStatic* actor;
		float creationTime;
		std::string uniqueName;
		std::string ownerName;
		float remainingLifetime;
	};
	std::vector<TrailSegment> trailSegments;
	float trailLifetime = 5.0f; // seconds
	unsigned int trailCounter = 0;
	float simulationTime = 0.0f; // running simulation time
	void updateTrailLifetime(float dt);
	void removeAllTrailSegmentsByOwner(const std::string& owner);
	void updateWinCondition(float dt);

	// Initialize PhysX and vehicles
	void initPhysX();
	void cleanupPhysX();
	void initGroundPlane();
	void initBoarder();
	void cleanupGroundPlane();
	void initMaterialFrictionTable();
	bool initVehicles(int numAI);
	bool initPhysics();
	bool pendingReinit = false;
	double reinitTime = 0.0;
	double reinitDelay = 3.0;  
public:
	void cleanupPhysics();
	// Ctor/Dtor
	PhysicsSystem(GameState& gameState, std::vector<Model> tModel);
	~PhysicsSystem();

	// add random obstacles
	void addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density = 10.f);

	// vehicle and trail related
	void addTrail(float x, float z, float rot, const char* name);

	// get global position of rigidDynamic list object
	physx::PxVec3 getPos(int i);

	// get transformList transforms
	Transform* getTransformAt(int i);

	// update the transforms based on physx
	void updateTransforms(std::vector<Entity>& entityList);

	// Shatters the car into cubes
	void shatter(physx::PxVec3 location, physx::PxVec3 direction);

	// updates collsions
	void updateCollisions();
	void removeDyanmicObject(uint32_t index);
	void removeStaticPhysicsObjects(std::string colliding1);
	void removeStaticEntityObjects(std::string colliding1);


	// resets map
	void reintialize();

	// run physx simulation and update the transforms
	void updatePhysics(double dt);

	// Do fixed step physics calculations
	void stepPhysics(float timestep, Command& command, Command& controllerCommand);

	bool getExplosion();
	glm::vec3 getExplosionLocation();
	float getCarSpeed(int i);
	float calculateEngineRPM(float speed);

	// Return positions of all AI vehicles
	std::vector<physx::PxVec3> getAIPositions();
	void update(double deltaTime);

	void reset();
	bool playerDied = false;
};