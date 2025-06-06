#include "PhysicsSystem.h"
#include "AudioEngine.h"
// Initialize PVD, create scene, vehicle compatibility, and scene components (gravity and friction)
void PhysicsSystem::initPhysX() {
	// namespaces
	using namespace physx; using namespace snippetvehicle2;

	// connect to localhost PVD
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	// Setup scene
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = gGravity;

	// Create CPU dispatchers
	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;

	/// NOTE: VehicleFilterShader has been modified. It handles callbacks related to collision incidents.
	// Create scene callbacks
	sceneDesc.filterShader = VehicleFilterShader;

	gContactReportCallback = new ContactReportCallback();
	sceneDesc.simulationEventCallback = gContactReportCallback;

	// Create scene
	gScene = gPhysics->createScene(sceneDesc);

	// Setup PVD client
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// Create a global material
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// Initialize vehicle compatibility
	PxInitVehicleExtension(*gFoundation);
}

// Clean up initPhysX
void PhysicsSystem::cleanupPhysX() {
	// namespace
	using namespace physx; using namespace snippetvehicle2;

	// detach and close vehicle extension
	PxCloseVehicleExtension();

	// free resources
	PX_RELEASE(gMaterial);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);

	// disconnect from PVD
	if (gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		PX_RELEASE(gPvd);
		PX_RELEASE(transport);
	}

	// Free scene
	PX_RELEASE(gFoundation);
}

// Create ground
void PhysicsSystem::initGroundPlane() {
	// namespace
	using namespace physx;

	// ground set at height 1
	gGroundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);

	// for each shape in the ground plane, set flags
	for (PxU32 i = 0; i < gGroundPlane->getNbShapes(); i++) {
		PxShape* shape = NULL;
		gGroundPlane->getShapes(&shape, 1, i);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	// add ground to scene
	gScene->addActor(*gGroundPlane);
}

// Release ground
void PhysicsSystem::cleanupGroundPlane() {
	gGroundPlane->release();
}

void PhysicsSystem::initBoarder() {

	// Dimensions
	float length = 100.0f;  // Length of the area
	float height = 1.0f;    // Height of the wall
	float thickness = 1.0f; // Thickness of the wall

	// Physics material
	physx::PxMaterial* pMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);

	// Collision filter
	physx::PxFilterData itemFilter(
		COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0
	);

	// Geometry of the wall
	physx::PxBoxGeometry wallGeom(length, height, thickness);

	// Wall positions and rotations
	struct WallData {
		physx::PxVec3 position;
		float rotationY; // Rotation around Y axis in radians
		int side;
	};

	std::vector<WallData> walls = {
		{{0, height, length}, 0.0f, 0},                          // Front wall
		{{0, height, -length}, 0.0f, 1},                         // Back wall
		{{length, height, 0}, physx::PxPi / 2, 2},               // Right wall
		{{-length, height, 0}, physx::PxPi / 2, 3}               // Left wall
	};

	for (const auto& wall : walls) {
		// Transform
		physx::PxTransform wallTransform(
			wall.position,
			physx::PxQuat(wall.rotationY, physx::PxVec3(0, 1, 0))
		);

		// Create shape
		physx::PxShape* shape = gPhysics->createShape(wallGeom, *pMaterial);
		shape->setSimulationFilterData(itemFilter);

		// Create body
		physx::PxRigidStatic* body = gPhysics->createRigidStatic(wallTransform);
		body->attachShape(*shape);
		body->setName("boundary");
		gScene->addActor(*body);

		// Rendering
		for (int i = -25; i < 25; i++) {
			if (gState.tempTrails) {
				gState.staticEntities.push_back(Entity("boarder", &pModels[6], new Transform()));
			}
			else {
				gState.staticEntities.push_back(Entity("boarder", &pModels[7], new Transform()));
			}
			//gState.staticEntities.push_back(Entity("boarder", &pModels[wall.side], new Transform()));
			if (wall.position.x == 0) {
				gState.staticEntities.back().transform->pos = glm::vec3(
					wallTransform.p.x + i * 4,
					wallTransform.p.y - 1,
					wallTransform.p.z
				);
			}
			else {
				gState.staticEntities.back().transform->pos = glm::vec3(
					wallTransform.p.x,
					wallTransform.p.y - 1,
					wallTransform.p.z + i * 4
				);
			}
			if (gState.tempTrails) {
				gState.staticEntities.back().transform->scale = glm::vec3(0.04);
				gState.staticEntities.back().transform->rot = glm::vec3(0, wall.rotationY, 0);
			}
			else {
				gState.staticEntities.back().transform->scale = glm::vec3(5);
				gState.staticEntities.back().transform->rot = glm::vec3(0, static_cast<float>(rand()), 0);
			}
		}


		// Clean up
		shape->release();
	}
	pMaterial = nullptr;
}


// Set the default game friction and material properties
void PhysicsSystem::initMaterialFrictionTable() {
	/*
	DESCRIPTION:
		Each physx material can be mapped to a tire friction value on a per tire basis.
		If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
		In this snippet there is only a single material so there can only be a single mapping between material and friction.
		In this snippet the same mapping is used by all tires.
	*/

	// set material parameters
	gPhysXMaterialFrictions[0].friction = 1.0f;
	gPhysXMaterialFrictions[0].material = gMaterial;
	gPhysXDefaultMaterialFriction = 1.0f;
	gNbPhysXMaterialFrictions = 1;
}

// Creates player and AI vehicles
bool PhysicsSystem::initVehicles(int numAI) {
	// namespace
	using namespace physx; using namespace snippetvehicle2;

	// hard limit the number of AI
	numAI = (numAI > 3) ? 3 : numAI;

	// Load parameters once (assuming all vehicles use the same params)
	snippetvehicle2::BaseVehicleParams baseParams;
	readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", baseParams);

	snippetvehicle2::PhysXIntegrationParams physxParams;
	setPhysXIntegrationParams(baseParams.axleDescription,
		gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
		physxParams);

	EngineDrivetrainParams engineDriveParams;
	readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json", engineDriveParams);

	// Create player + Number of AI
	for (int i = 0; i < numAI + 1; i++) {
		Vehicle* vehicle = new Vehicle();

		// Set parameters
		if (i == 0) {
			vehicle->name = "playerVehicle";
		}
		else {
			vehicle->name = "vehicle" + std::to_string(i);
		}
		vehicle->vehicle.mBaseParams = baseParams;
		vehicle->vehicle.mPhysXParams = physxParams;
		vehicle->vehicle.mEngineDriveParams = engineDriveParams;

		// Initialize the vehicle
		if (!vehicle->vehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE)) {
			return false;
		}

		// Set the vehicle position, rotation and model
		PxTransform pose;

		if (i == 0) {
			pose = PxTransform(PxVec3(80.f, 0.f, -80.f), PxQuat(0, PxVec3(0, 1, 0))); // bottom right
		}
		else if (i == 1) {
			pose = PxTransform(PxVec3(80.f, 0.f, 80.f), PxQuat(3.1415, PxVec3(0, 1, 0))); // top right
		}
		else if (i == 2) {
			pose = PxTransform(PxVec3(-80.f, 0.f, 80.f), PxQuat(3.1415, PxVec3(0, 1, 0))); // top left
		}
		else {
			pose = PxTransform(PxVec3(-80.f, 0.f, -80.f), PxQuat(0, PxVec3(0, 1, 0))); // bottom left
		}

		// create a physX actor
		vehicle->vehicle.setUpActor(*gScene, pose, vehicle->name.c_str());

		// Add to rigid dynamic and transform lists
		PxRigidBody* rigidBody = vehicle->vehicle.mPhysXState.physxActor.rigidBody;
		PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(rigidBody);
		rigidDynamicList.push_back(rigidDynamic);
		transformList.push_back(new Transform());

		// Set collision filter
		PxFilterData vehicleFilter(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

		// Get scale and set flags for each item 
		PxU32 shapes = rigidBody->getNbShapes();
		for (PxU32 j = 0; j < shapes; j++) {
			PxShape* shape = nullptr;
			rigidBody->getShapes(&shape, 1, j);

			// Get dimensions of the car
			if (shape->getGeometry().getType() == PxGeometryType::eBOX) {
				const PxBoxGeometry& box = static_cast<const PxBoxGeometry&>(shape->getGeometry());
				//transformList.back()->scale = glm::vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
			}

			// Set flags
			shape->setSimulationFilterData(vehicleFilter);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		}

		// set max speed
		vehicle->vehicle.mPhysXState.physxActor.rigidBody->setMaxLinearVelocity(10);

		// Set initial gear states
		vehicle->vehicle.mEngineDriveState.gearboxState.currentGear = vehicle->vehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
		vehicle->vehicle.mEngineDriveState.gearboxState.targetGear = vehicle->vehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
		vehicle->vehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

		// Set initial previous position and direction
		vehicle->prevPos = pose.p;
		vehicle->prevDir = pose.q.getBasisVector2();

		// Set player vehicle state (assuming first vehicle is the player's)
		if (i == 0) {
			gState.playerVehicle.curPos = vehicle->prevPos;
			gState.playerVehicle.curDir = vehicle->prevDir.getNormalized();
		}

		// update the dynamicEntity list
		if (i == 0) {
			gState.dynamicEntities.emplace_back("playerCar", &pModels[4], transformList.back());
			gState.dynamicEntities.back().vehicle = vehicle;
		}
		else {
			if (i == 1) gState.dynamicEntities.emplace_back("aiCar1", &pModels[4], transformList.back());
			if (i == 2) gState.dynamicEntities.emplace_back("aiCar2", &pModels[4], transformList.back());
			if (i == 3) gState.dynamicEntities.emplace_back("aiCar3", &pModels[4], transformList.back());
			gState.dynamicEntities.back().vehicle = vehicle;
		}
	}

	// Set up simulation context (only needs to be done once)
	gVehicleSimulationContext.setToDefault();
	gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
	gVehicleSimulationContext.scale.scale = 1.0f;
	gVehicleSimulationContext.gravity = gGravity;
	gVehicleSimulationContext.physxScene = gScene;
	gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;

	return true;
}

bool PhysicsSystem::initPhysics() {
	initPhysX();
	initGroundPlane();
	initBoarder();
	initMaterialFrictionTable();
	if (!initVehicles(3))
		return false;
	return true;
}

void PhysicsSystem::cleanupPhysics() {
	for (auto& entity : gState.dynamicEntities) {
		if (entity.vehicle != nullptr) {
			entity.vehicle->vehicle.destroy();
		}
	}
	cleanupGroundPlane();
	cleanupPhysX();

	delete gContactReportCallback;
	gContactReportCallback = nullptr;
}

PhysicsSystem::PhysicsSystem(GameState& gameState, std::vector<Model> tModel) :
	gState(gameState), pModels(tModel) {
	initPhysics();
}

PhysicsSystem::~PhysicsSystem() {
	cleanupPhysics();
}

void PhysicsSystem::addTrail(float x, float z, float rot, const char* name) {
	physx::PxMaterial* pMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);

	// create shape
	float height = .5f;
	float width = 0.01f;
	physx::PxBoxGeometry geom(trailStep / 2, height, width);
	physx::PxShape* shape = gPhysics->createShape(geom, *pMaterial);

	physx::PxTransform wallTransform(
		physx::PxVec3(x, height, z),
		physx::PxQuat(rot, physx::PxVec3(0, 1, 0))
	);
	physx::PxRigidStatic* body = gPhysics->createRigidStatic(wallTransform);

	// update shape and attach
	physx::PxFilterData itemFilter(
		COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0
	);
	shape->setSimulationFilterData(itemFilter);

	body->attachShape(*shape);
	body->setName(name);

	int modelType = 0;
	if (strcmp(name, "vehicle1") == 0) modelType = 1;
	if (strcmp(name, "vehicle2") == 0) modelType = 2;
	if (strcmp(name, "vehicle3") == 0) modelType = 3;

	gState.staticEntities.push_back(Entity(name, &pModels[modelType], new Transform()));

	gState.staticEntities.back().transform->pos = glm::vec3(
		wallTransform.p.x,
		wallTransform.p.y - 0.3,
		wallTransform.p.z
	);

	gState.staticEntities.back().transform->scale = glm::vec3(trailStep / 2, height, 1.3);

	gState.staticEntities.back().transform->rot = glm::vec3(0, static_cast<float>(rand()), 0);


	gScene->addActor(*body);

	// Clean up
	shape->release();
	pMaterial = nullptr;

	TrailSegment segment;
	segment.actor = body;
	segment.creationTime = simulationTime;
	segment.uniqueName = name;
	segment.ownerName = name;
	segment.remainingLifetime = trailLifetime;
	trailSegments.push_back(segment);

	// add the add the block to the graph
	physx::PxVec3 dir = wallTransform.q.getBasisVector0();
	physx::PxVec3 start = wallTransform.p - .5f * trailStep * dir;
	physx::PxVec3 end = wallTransform.p + .5f * trailStep * dir;

	gState.gMap.updateMap({ start.x, start.z }, { end.x, end.z });

}

physx::PxVec3 PhysicsSystem::getPos(int i) {
	physx::PxVec3 position = rigidDynamicList[i]->getGlobalPose().p;
	return position;
}

Transform* PhysicsSystem::getTransformAt(int i) { return transformList[i]; }

Transform* PhysicsSystem::getTransformAt(std::string name) {
	Transform* c_name = nullptr;
	for (int i = 0; i < gState.dynamicEntities.size(); i++) {
		//std::cout << gState.dynamicEntities[i].name << std::endl;
		if (gState.dynamicEntities[i].name == name) {
			c_name = gState.dynamicEntities[i].transform;
		}
	}
	return c_name;
}

glm::vec3 PhysicsSystem::getCarPos(std::string name) {
	for (int i = 0; i < gState.dynamicEntities.size(); i++) {
		if (gState.dynamicEntities[i].name == name) {
			return gState.dynamicEntities[i].transform->pos;
		}
	}
	return glm::vec3(0, 0, 0);
}

void PhysicsSystem::updateTransforms(std::vector<Entity>& entityList) {
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

void PhysicsSystem::addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density) {
	physx::PxMaterial* pMaterial = gPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.restitution);

	// Define a item
	physx::PxShape* shape = gPhysics->createShape(*geom, *pMaterial);
	physx::PxTransform tran(physx::PxVec3(0));
	physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(tran.transform(transform));

	// Add to lists
	rigidDynamicList.push_back(body);
	transformList.push_back(new Transform());

	// update shape and attach
	physx::PxFilterData itemFilter(
		COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(itemFilter);
	body->attachShape(*shape);

	physx::PxRigidBodyExt::updateMassAndInertia(*body, density);
	gScene->addActor(*body);

	// Clean up
	shape->release();
	pMaterial = nullptr;
}

void PhysicsSystem::shatter(physx::PxVec3 location, physx::PxVec3 direction) {

	physx::PxMaterial* pMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	physx::PxBoxGeometry boxGeom = physx::PxBoxGeometry(0.25, 0.25, 0.25);

	// Define a item
	physx::PxShape* shape = gPhysics->createShape(boxGeom, *pMaterial);
	physx::PxTransform tran(physx::PxVec3(0));

	for (int i = 0; i < 20; i++) {
		physx::PxTransform localTran(location.x, location.y + i * 0.001, location.z);
		physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(tran.transform(localTran));

		// Add to lists
		rigidDynamicList.push_back(body);
		transformList.push_back(new Transform());

		// update shape and attach
		physx::PxFilterData itemFilter(
			0,
			COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
		shape->setSimulationFilterData(itemFilter);
		body->attachShape(*shape);
		body->setName("scrap");

		physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		gScene->addActor(*body);

		//body->addForce(physx::PxVec3(0, 0, 0));
		// Clean up
		shape->release();
		pMaterial = nullptr;

		gState.dynamicEntities.emplace_back(Entity("scrap", &pModels[5], transformList.back()));
		gState.dynamicEntities.back().transform->scale = glm::vec3(0.5, 0.5, 0.5);
	}
}

void PhysicsSystem::updateCollisions() {
	auto collisionPair = gContactReportCallback->getCollisionPair();
	if (gContactReportCallback->checkCollision()) {
		std::string colliding1 = collisionPair.first->getName();
		std::string colliding2 = collisionPair.second->getName();
		//std::cout << colliding1 << std::endl;
		if (colliding1 == "playerVehicle") {
			deadCars[0] = 1;
			playerDied = true;
		}
		if (colliding1 == "vehicle1") {
			deadCars[1] = 1;
		    player2Died = true;
		}
		if (colliding1 == "vehicle2") {
			deadCars[2] = 1;
			player3Died = true;
		}
		if (colliding1 == "vehicle3") {
			deadCars[3] = 1;
			player4Died = true;
		}

		for (int i = gState.dynamicEntities.size() - 1; i >= 0; i--) {
			auto& entity = gState.dynamicEntities[i];
			if ((entity.name != "aiCar1" && entity.name != "aiCar2" && entity.name != "aiCar3") && entity.name != "playerCar") continue;
			if (entity.vehicle->name == colliding1) {
				shatter(entity.vehicle->prevPos, entity.vehicle->prevDir);
				entity.vehicle->vehicle.destroy();

				for (int x = 0; x <= transformList.size(); x++) {
					if (entity.transform == transformList[x]) {
						rigidDynamicList.erase(rigidDynamicList.begin() + x);
						transformList.erase(transformList.begin() + x);
						break;
					}
				}
				gState.dynamicEntities.erase(gState.dynamicEntities.begin() + i);

				PxU32 actorCount = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
				std::vector<PxActor*> actors(gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
				gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);
				for (PxActor* actor : actors) {
					const char* actorName = actor->getName();
					if (actorName) {
						if (actorName == colliding1) {
							gScene->removeActor(*actor);
						}
					}
				}
				for (int g = gState.staticEntities.size() - 1; g >= 0; g--) {
					if (gState.staticEntities[g].name == colliding1) {
						gState.staticEntities.erase(gState.staticEntities.begin() + g);
					}
				}
				if (colliding1 != colliding2) {
					gState.addScoreToVehicle(colliding2, 1);
				}
			}
		}

		// Helper lambda to pick random element
		auto pickRandomAliveAI = [this]() -> std::string {
			std::vector<std::string> aliveAIs;
			for (auto& entity : gState.dynamicEntities) {
				if (entity.name == "aiCar1" || entity.name == "aiCar2" || entity.name == "aiCar3") {
					aliveAIs.push_back(entity.vehicle->name);
				}
			}
			if (!aliveAIs.empty()) {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> dis(0, aliveAIs.size() - 1);
				return aliveAIs[dis(gen)];
			}
			return ""; // No alive AI
		};

		int totalDead = playerDied + player2Died + player3Died + player4Died;
		if (!gState.splitScreenEnabled && !gState.splitScreenEnabled4) {
			if (playerDied) {
				pendingReinit = true;
				reinitTime = 0.0;
				std::string randomAI = pickRandomAliveAI();
				if (!randomAI.empty()) {
					gState.addScoreToVehicle(randomAI, 1);
				}
			}
			if (totalDead >= 3) {
				pendingReinit = true;
				reinitTime = 0.0;
				gState.addScoreToVehicle("playerVehicle", 1);
			}
		}
		else if (gState.splitScreenEnabled) {
			if (playerDied && player2Died) {
				pendingReinit = true;
				reinitTime = 0.0;
				std::string randomAI = pickRandomAliveAI();
				if (!randomAI.empty()) {
					gState.addScoreToVehicle(randomAI, 1);
				}
			}
			if (totalDead >= 3) {
				pendingReinit = true;
				reinitTime = 0.0;
				if (!playerDied) gState.addScoreToVehicle("playerVehicle", 1);
				if (!player2Died) gState.addScoreToVehicle("vehicle1", 1);
			}
			
		}
		else if (gState.splitScreenEnabled4) {
			if (totalDead >= 3) {
				pendingReinit = true;
				reinitTime = 0.0;
				// Award to the last vehicle standing
				if (!playerDied) gState.addScoreToVehicle("playerVehicle", 1);
				if (!player2Died) gState.addScoreToVehicle("vehicle1", 1);
				if (!player3Died) gState.addScoreToVehicle("vehicle2", 1);
				if (!player4Died) gState.addScoreToVehicle("vehicle3", 1);
			}
		}

		gContactReportCallback->readNewCollision();
	}
}


void PhysicsSystem::reintialize() {
	for (int i = gState.dynamicEntities.size() - 1; i >= 0; i--) {
		auto& entity = gState.dynamicEntities[i];
		if (entity.name == "playerCar" || (entity.name == "aiCar1" || entity.name == "aiCar2" || entity.name == "aiCar3")) {
			entity.vehicle->vehicle.destroy();
		}

		// remove Dynamic Objects
		rigidDynamicList.erase(rigidDynamicList.begin() + i);
		transformList.erase(transformList.begin() + i);
		gState.dynamicEntities.erase(gState.dynamicEntities.begin() + i);

		PxU32 actorCount = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
		std::vector<PxActor*> actors(gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
		gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors.data(), actorCount);
		for (PxActor* actor : actors) {
			const char* actorName = actor->getName();
			if (actorName) {
				if (actorName == "scrap") {
					gScene->removeActor(*actor);
				}
			}
		}
	}

	// reset fuel
	gState.playerVehicle.fuel =  -1;
	gState.playerVehicle2.fuel = -1;
	gState.playerVehicle3.fuel = -1;
	gState.playerVehicle4.fuel = -1;


	PxU32 actorCount = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	std::vector<PxActor*> actors(gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
	gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, actors.data(), actorCount);
	for (int f = actors.size() - 1; f >= 5; f--) {
		const char* actorName = actors[f]->getName();
		if (actorName) {
			gScene->removeActor(*actors[f]);
		}
	}

	// Remove all static entity objects
	for (int g = gState.staticEntities.size() - 1; g >= 0; g--) {
		if (gState.staticEntities[g].name != "boarder") {
			gState.staticEntities.erase(gState.staticEntities.begin() + g);
		}
	}

	// -- Update random scores
	// two player
	if (gState.splitScreenEnabled) {
		// if both ai are alive
		if (!player3Died && !player4Died) {
			int chance = rand() % 5;

			// 0,1
			if (chance <= 1) {
				gState.addScoreToVehicle("vehicle2", 1);
			}
			// 2, 3
			else if (chance <= 3) {
				gState.addScoreToVehicle("vehicle3", 1);
			}
		}
	}
	// single player
	else if (!gState.splitScreenEnabled4 && playerDied) {
		// get number of players alive
		int alive = !player2Died + !player3Died + !player4Died;

		// two alive
		if (alive == 2) {
			// get the names
			std::vector<string> names;
			if (!player2Died) {
				names.push_back("vehicle1");
			}
			if (!player3Died) {
				names.push_back("vehicle2");
			}
			if (!player4Died) {
				names.push_back("vehicle3");
			}

			// get the odds
			int chance = rand() % 5;
			
			// if 4/5 odds
			if (chance < 4) {
				// bin down to 0,1
				chance /= 2;
				// add score to the name
				gState.addScoreToVehicle(names[chance], 1);
			}
		}

		// 3 alive
		else if (alive == 3) {
			for (int i = 0; i <= 1; i++) {
				// get number
				int chance = rand() % 7;

				// add scores
				if (chance <= 1) {
					gState.addScoreToVehicle("vehicle1", 1);
				}
				else if (chance <= 3) {
					gState.addScoreToVehicle("vehicle2", 1);
				}
				else if (chance <= 5) {
					gState.addScoreToVehicle("vehicle3", 1);
				}
			}
		}
	}

	//std::cout << gState.dynamicEntities.size() << " " << gState.staticEntities.size() << " " << actors.size() << "\n";
	gState.gMap.resetMap();
	m_stationaryTime.clear();
	initVehicles(3);
}

void PhysicsSystem::updatePhysics(double dt) {
	gScene->simulate(dt);
	gScene->fetchResults(true);

	updateTransforms(gState.dynamicEntities);

	if (gState.tempTrails) updateTrailLifetime(dt);
}

void PhysicsSystem::stepPhysics(float timestep, Command& keyboardCommand, const std::vector<Command*>& playerCommands) {
	using namespace physx;
	using namespace snippetvehicle2;

	for (auto& entity : gState.dynamicEntities) {
		bool isPlayerControlled = false;
		int playerIndex = -1;
		std::string name = entity.name;

		if (!gState.splitScreenEnabled && !gState.splitScreenEnabled4) {
			// 1-player mode:
			if (name == "playerCar") {
				isPlayerControlled = true;
				playerIndex = 0;
			}
		}
		else if (gState.splitScreenEnabled && !gState.splitScreenEnabled4) {
			// 2-player mode
			if (name == "playerCar") {
				isPlayerControlled = true;
				playerIndex = 0;
			}
			else if (name == "aiCar1") {
				isPlayerControlled = true;
				playerIndex = 1;
			}
		}
		else if (gState.splitScreenEnabled4) {
			// 4-player mode:
			if (name == "playerCar") {
				isPlayerControlled = true;
				playerIndex = 0;
			}
			else if (name == "aiCar1") {
				isPlayerControlled = true;
				playerIndex = 1;
			}
			else if (name == "aiCar2") {
				isPlayerControlled = true;
				playerIndex = 2;
			}
			else if (name == "aiCar3") {
				isPlayerControlled = true;
				playerIndex = 3;
			}
		}

		if (isPlayerControlled) {
			Command cmd;
			bool playerIsDead = false;
			if (playerIndex == 0) playerIsDead = playerDied;
			else if (playerIndex == 1) playerIsDead = player2Died;
			else if (playerIndex == 2) playerIsDead = player3Died;
			else if (playerIndex == 3) playerIsDead = player4Died;

			// reset fuel
			if (playerIndex == 0 && gState.playerVehicle.fuel == -1) {
				keyboardCommand.reset();
				playerCommands[playerIndex]->reset();
			} else if (playerIndex == 1 && gState.playerVehicle2.fuel == -1) {
				playerCommands[playerIndex]->reset();
			} else if (playerIndex == 2 && gState.playerVehicle3.fuel == -1) {
				playerCommands[playerIndex]->reset();
			} else if (playerIndex == 3 && gState.playerVehicle4.fuel == -1) {
				playerCommands[playerIndex]->reset();
			}

			if (playerIsDead) {
				cmd.brake = 1.0f;
				cmd.throttle = 0.0f;
				cmd.steer = 0.0f;
				cmd.fuel = 1;
				cmd.boost = false;
				playerCommands[playerIndex]->fuel = 1;
				if (playerIndex == 0) keyboardCommand.fuel = 1;
				entity.vehicle->setPhysxCommand(cmd);
			}
			else {
				//keyboard only works for player 1
				if (playerIndex == 0) {
					cmd.brake = PxMax(keyboardCommand.brake, playerCommands[0]->brake);
					cmd.throttle = 0.5f + PxMax(keyboardCommand.throttle, playerCommands[0]->throttle) / 2.f;
					cmd.steer = (fabs(keyboardCommand.steer) > fabs(playerCommands[0]->steer)) ? keyboardCommand.steer : playerCommands[0]->steer;
					cmd.fuel = std::min(keyboardCommand.fuel, playerCommands[0]->fuel);
					cmd.boost = (playerCommands[0]->boost == false) ? keyboardCommand.boost : playerCommands[0]->boost;
					entity.vehicle->setPhysxCommand(cmd);
				}
				else {
					cmd.brake = playerCommands[playerIndex]->brake;
					cmd.throttle = 0.5f + playerCommands[playerIndex]->throttle / 2.f;
					cmd.steer = playerCommands[playerIndex]->steer;
					cmd.fuel = playerCommands[playerIndex]->fuel;
					cmd.boost = playerCommands[playerIndex]->boost;
					entity.vehicle->setPhysxCommand(cmd);
				}
			}
			// Step the vehicle
			entity.vehicle->forward = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
			entity.vehicle->velocity = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
			PxReal forwardSpeed = entity.vehicle->velocity.dot(entity.vehicle->forward);
			const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);

			// boost
			physx::PxRigidBody* rigidBody = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody;
			if (cmd.boost && cmd.fuel > 0) {
				rigidBody->setMaxLinearVelocity(100);
				rigidBody->addForce(entity.vehicle->forward * 20, PxForceMode::eACCELERATION);
			}
			else {
				physx::PxReal curr_max = rigidBody->getMaxLinearVelocity() - 250 * timestep;
				curr_max = (curr_max < 10) ? 10 : curr_max;

				// brake
				if (cmd.brake != 0) {
					rigidBody->setMaxLinearVelocity(10.f - 4.f * cmd.brake);
				}

				else {
					rigidBody->setMaxLinearVelocity(curr_max);
				}
			}
			// update fuel
			if (playerIndex == 0) gState.playerVehicle.fuel = cmd.fuel;
			if (playerIndex == 1) gState.playerVehicle2.fuel = cmd.fuel;
			if (playerIndex == 2) gState.playerVehicle3.fuel = cmd.fuel;
			if (playerIndex == 3) gState.playerVehicle4.fuel = cmd.fuel;
			playerCommands[playerIndex]->updateBoost(timestep);
			if (playerIndex == 0) keyboardCommand.updateBoost(timestep);

			// run physx simulation
			entity.vehicle->vehicle.mComponentSequence.setSubsteps(entity.vehicle->vehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
			entity.vehicle->vehicle.step(timestep, gVehicleSimulationContext);

			// Update trails
			const PxVec3 currPos = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
			PxVec3 travel = currPos - entity.vehicle->prevPos;
			int steps = travel.magnitude() / trailStep;

			for (int i = 0; i < steps; i++) {
				float ratio = float(i + 1) / float(steps);

				physx::PxVec3 travNorm = ratio * entity.vehicle->prevDir.getNormalized() + (1 - ratio) * travel.getNormalized();
				physx::PxVec3 placementLoc = entity.vehicle->prevPos - 1.2f * gState.dynamicEntities.at(0).transform->scale.x * travNorm;
				addTrail(placementLoc.x, placementLoc.z, -atan2(travNorm.z, travNorm.x), entity.vehicle->name.c_str());

				entity.vehicle->prevPos += trailStep * travel.getNormalized();

				if (i + 1 == steps) {
					entity.vehicle->prevDir = travel;
				}
			}

			// Update player vehicle state
			if (playerIndex == 0) {
				gState.playerVehicle.curPos = currPos;
				gState.playerVehicle.curDir = entity.vehicle->forward.getNormalized();
			}
			if (playerIndex == 1) {
				gState.playerVehicle2.curPos = currPos;
				gState.playerVehicle2.curDir = entity.vehicle->forward.getNormalized();
			}
			if (playerIndex == 2) {
				gState.playerVehicle3.curPos = currPos;
				gState.playerVehicle3.curDir = entity.vehicle->forward.getNormalized();
			}
			if (playerIndex == 3) {
				gState.playerVehicle4.curPos = currPos;
				gState.playerVehicle4.curDir = entity.vehicle->forward.getNormalized();
			}
		}
		if (!isPlayerControlled && (entity.name == "aiCar1" || entity.name == "aiCar2" || entity.name == "aiCar3")) {
			entity.vehicle->update(gState, *this);

			// Step the vehicle
			entity.vehicle->forward = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
			entity.vehicle->velocity = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
			const PxReal forwardSpeed = entity.vehicle->velocity.dot(entity.vehicle->forward);
			const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);

			// boost
			physx::PxRigidBody* rigidBody = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody;
			if (entity.vehicle->command.boost && entity.vehicle->command.fuel > 0) {
				rigidBody->setMaxLinearVelocity(100);
				rigidBody->addForce(entity.vehicle->forward * 20, PxForceMode::eACCELERATION);
			}
			else {
				physx::PxReal curr_max = rigidBody->getMaxLinearVelocity() - 250 * timestep;
				curr_max = (curr_max < 10) ? 10 : curr_max;

				if (entity.vehicle->command.brake != 0) {
					rigidBody->setMaxLinearVelocity(10.f - 4.f * entity.vehicle->command.brake);
				}
				else {
					rigidBody->setMaxLinearVelocity(curr_max);
				}
			}
			entity.vehicle->command.updateBoost(timestep);

			// run physx simulation
			entity.vehicle->vehicle.mComponentSequence.setSubsteps(entity.vehicle->vehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
			entity.vehicle->vehicle.step(timestep, gVehicleSimulationContext);

			// Update trails
			const PxVec3 currPos = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
			PxVec3 travel = currPos - entity.vehicle->prevPos;
			int steps = travel.magnitude() / trailStep;

			for (int i = 0; i < steps; i++) {
				float ratio = float(i + 1) / float(steps);

				physx::PxVec3 travNorm = ratio * entity.vehicle->prevDir.getNormalized() + (1 - ratio) * travel.getNormalized();
				physx::PxVec3 placementLoc = entity.vehicle->prevPos - 1.2f * gState.dynamicEntities.at(0).transform->scale.x * travNorm;
				addTrail(placementLoc.x, placementLoc.z, -atan2(travNorm.z, travNorm.x), entity.vehicle->name.c_str());

				entity.vehicle->prevPos += trailStep * travel.getNormalized();

				if (i + 1 == steps) {
					entity.vehicle->prevDir = travel;
				}
			}
		}
	}

	updateCollisions();

	// Simulate the entire PhysX scene
	simulationTime += timestep;
	gScene->simulate(timestep);
	gScene->fetchResults(true);
	if(gState.tempTrails) updateTrailLifetime(timestep);
	updateTrailSize();
	handleStalledVehicles(timestep);
}

bool PhysicsSystem::getExplosion() {
	return gContactReportCallback->checkCollision();
}

glm::vec3 PhysicsSystem::getExplosionLocation() {
	for (const auto& entity : gState.dynamicEntities) {
		if (entity.name == "playerCar" || (entity.name == "aiCar1" || entity.name == "aiCar2" || entity.name == "aiCar3")) {
			if (entity.vehicle->name == gContactReportCallback->getCollisionPair().first->getName()) {
				physx::PxVec3 pos = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
				return glm::vec3(pos.x, pos.y, pos.z);
			}
		}

	}
	return glm::vec3(0, 0, 0);
}

float PhysicsSystem::getCarSpeed(int i) {
	using namespace physx;
	// Ensure there is at least one vehicle (assumed to be the player's vehicle)
	if (gState.dynamicEntities.size() == 0 || gState.dynamicEntities[i].name == "scrap") {
		return 0.0f;
	}

	// Retrieve the player's vehicle rigid body.
	PxRigidBody* playerRigidBody = gState.dynamicEntities[i].vehicle->vehicle.mPhysXState.physxActor.rigidBody;

	// Get the vehicle's current linear velocity.
	PxVec3 linVel = playerRigidBody->getLinearVelocity();

	// Get the vehicle's forward direction.
	// Here, getBasisVector2() is used assuming that the forward (longitudinal) axis is basis vector 2.
	PxVec3 forwardDir = playerRigidBody->getGlobalPose().q.getBasisVector2();

	// The forward speed is the component of linear velocity in the forward direction.
	return linVel.dot(forwardDir);
}

float PhysicsSystem::calculateEngineRPM(float speed) {
	// Define idle and maximum RPM values.
	const float idleRPM = 800.0f;
	const float maxRPM = 7000.0f;
	// Define an assumed maximum speed (m/s) at which the engine reaches max RPM.
	// Adjust this value to suit your game's balance.
	const float maxSpeed = 50.0f;

	// Clamp the speed between 0 and maxSpeed to avoid overshooting.
	speed = std::max(0.0f, std::min(speed, maxSpeed));

	// Calculate engine RPM with a simple linear interpolation.
	float rpm = idleRPM + ((maxRPM - idleRPM) * (speed / maxSpeed));
	return rpm;
}

std::vector<physx::PxVec3> PhysicsSystem::getAIPositions() {
	std::vector<physx::PxVec3> aiPositions;
	// Loop through all dynamic entities
	for (const auto& entity : gState.dynamicEntities) {
		// Only process AI vehicles (skip the player)
		if ((entity.name == "aiCar1" || entity.name == "aiCar2" || entity.name == "aiCar3") && entity.vehicle != nullptr) {
			// Retrieve the global position from the vehicle's rigid body
			physx::PxVec3 pos = entity.vehicle->vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
			aiPositions.push_back(pos);
		}
	}
	return aiPositions;
}

void PhysicsSystem::updateTrailLifetime(float dt) {
	// Update the running simulation time.
	// Iterate backward through the trail segments.
	for (int i = trailSegments.size() - 1; i >= 0; i--) {
		if (simulationTime - trailSegments[i].creationTime >= trailLifetime) {
			physx::PxTransform pose;
			if (trailSegments[i].actor) {
				pose = trailSegments[i].actor->getGlobalPose();
				if (trailSegments[i].actor->getScene() == gScene) {
					gScene->removeActor(*trailSegments[i].actor);
				}
				trailSegments[i].actor->release();
			}

			// Update map
			physx::PxVec3 dir = pose.q.getBasisVector0();
			float halfStep = trailStep / 2.0f;
			physx::PxVec3 startVec = pose.p - halfStep * dir;
			physx::PxVec3 endVec = pose.p + halfStep * dir;
			gState.gMap.updateMap({ startVec.x, startVec.z }, { endVec.x, endVec.z }, 1);

			// remove trail based on position now instead of a unique name
			for (auto it = gState.staticEntities.begin(); it != gState.staticEntities.end(); ) {
				glm::vec3 entityPos = it->transform->pos;
				if (fabs(entityPos.x - pose.p.x) < 0.5f && fabs(entityPos.z - pose.p.z) < 0.5f) {
					it = gState.staticEntities.erase(it);
				}
				else {
					++it;
				}
			}

			// Erase segment
			trailSegments.erase(trailSegments.begin() + i);

		}
	}
}

void PhysicsSystem::updateTrailSize() {
	float minSize = 0.5f;
	float maxSize = 1.0f;
	float rate = 0.5f;
	float scale;
	for (int i = trailSegments.size() - 1; i >= 0; i--) {
		physx::PxTransform pose;
		pose = trailSegments[i].actor->getGlobalPose();
		for (auto& entity : gState.staticEntities) {
			glm::vec3 entityPos = entity.transform->pos;
			if (fabs(entityPos.x - pose.p.x) < 0.5f && fabs(entityPos.z - pose.p.z) < 0.5f) {
				scale = std::min((simulationTime - trailSegments[i].creationTime)* rate + minSize, maxSize);
				entity.transform->scale = glm::vec3(scale);
			}
		}

	}
}

void PhysicsSystem::removeAllTrailSegmentsByOwner(const std::string& owner)
{
	// Iterate backward through the trail segments.
	for (int i = trailSegments.size() - 1; i >= 0; i--) {
		if (trailSegments[i].ownerName == owner) {
			// Recompute the start and end positions from the actor's transform.
			if (trailSegments[i].actor) {
				physx::PxTransform pose = trailSegments[i].actor->getGlobalPose();
				// Use the same logic as in addTrail:
				physx::PxVec3 dir = pose.q.getBasisVector0();
				float halfStep = trailStep / 2.0f;
				physx::PxVec3 startVec = pose.p - halfStep * dir;
				physx::PxVec3 endVec = pose.p + halfStep * dir;
				gState.gMap.updateMap({ startVec.x, startVec.z }, { endVec.x, endVec.z });
			}

			// Remove the actor from the scene if it is still there.
			if (trailSegments[i].actor && trailSegments[i].actor->getScene() == gScene) {
				gScene->removeActor(*trailSegments[i].actor);
				trailSegments[i].actor->release();
			}

			// Remove the corresponding static entity from the render list.
			for (auto it = gState.staticEntities.begin(); it != gState.staticEntities.end(); ) {
				if (it->name == trailSegments[i].uniqueName) {
					gState.gMap.updateMap(
						it->start,
						it->end,
						1
					);
					it = gState.staticEntities.erase(it);
				}
				else
					++it;
			}

			// Erase this trail segment from the vector.
			trailSegments.erase(trailSegments.begin() + i);
		}
	}
}

void PhysicsSystem::update(double deltaTime) {
	// Handle countdown before reinitialization
	if (pendingReinit) {
		reinitTime += deltaTime;

		if (reinitTime >= reinitDelay) {
			reintialize();
			pendingReinit = false;
			reinitTime = 0.0;
			playerDied = false;
			player2Died = false;
			player3Died = false;
			player4Died = false;
			// Reset the deadCars array
			std::fill(std::begin(deadCars), std::end(deadCars), 0);
		}
		return;  // Skip further updates while waiting for reinit
	}

	//updateCollisions();
}

void PhysicsSystem::handleStalledVehicles(float timestep) {
	std::vector<std::size_t> stalledIndices;

	for (std::size_t i = 0; i < gState.dynamicEntities.size(); ++i)
	{
		auto& entity = gState.dynamicEntities[i];
		if (!entity.vehicle) continue;

		float speed = getCarSpeed(static_cast<int>(i));
		auto& timer = m_stationaryTime[entity.vehicle->name];
		timer = (speed < kStallSpeed) ? timer + timestep : 0.f;

		if (timer >= kStallLimit)
			stalledIndices.push_back(i);
	}

	for (auto it = stalledIndices.rbegin(); it != stalledIndices.rend(); ++it) destroyVehicleAt(*it);
}

void PhysicsSystem::destroyVehicleAt(std::size_t i) {
	auto& entity = gState.dynamicEntities[i];
	std::string name = entity.name;
	//std::cout << name << " got stuck" << std::endl;

	if (name == "playerCar") {
		deadCars[0] = 1;
		playerDied = true;
	}
	if (name == "aiCar1") {
		deadCars[1] = 1;
		player2Died = true;
	}
	if (name == "aiCar2") {
		deadCars[2] = 1;
		player3Died = true;
	}
	if (name == "aiCar3") {
		deadCars[3] = 1;
		player4Died = true;
	}

	shatter(entity.vehicle->prevPos, entity.vehicle->prevDir);
	entity.vehicle->vehicle.destroy();

	transformList.erase(transformList.begin() + i);
	rigidDynamicList.erase(rigidDynamicList.begin() + i);

	gState.dynamicEntities.erase(gState.dynamicEntities.begin() + i);

	const int deadCount = playerDied + player2Died + player3Died + player4Died;

	if (!gState.splitScreenEnabled && !gState.splitScreenEnabled4)
	{
		if (playerDied || deadCount >= 3)
			pendingReinit = true;
	}
	else if (gState.splitScreenEnabled)
	{
		if ((playerDied && player2Died) || deadCount >= 3)
			pendingReinit = true;
	}
	else if (gState.splitScreenEnabled4)
	{
		if (deadCount >= 3)
			pendingReinit = true;
	}

	if (pendingReinit) reinitTime = 0.0;
}