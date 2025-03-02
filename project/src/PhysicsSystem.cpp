#include "PhysicsSystem.h"

void PhysicsSystem::initPhysX() {
	using namespace physx;
	using namespace snippetvehicle2;

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = gGravity;

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;

	/// <NOTICE>
	/// VehicleFilterShader has been modified. It handles callbacks related to collision
	/// incidents.
	/// </NOTICE>
	sceneDesc.filterShader = VehicleFilterShader;

	gContactReportCallback = new ContactReportCallback();
	sceneDesc.simulationEventCallback = gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	//gScene->setContactModifyCallback();
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxInitVehicleExtension(*gFoundation);
}

void PhysicsSystem::cleanupPhysX() {
	using namespace physx;
	using namespace snippetvehicle2;

	PxCloseVehicleExtension();

	PX_RELEASE(gMaterial);
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		PX_RELEASE(gPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

void PhysicsSystem::initGroundPlane() {
	using namespace physx;

	gGroundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	for (PxU32 i = 0; i < gGroundPlane->getNbShapes(); i++)
	{
		PxShape* shape = NULL;
		gGroundPlane->getShapes(&shape, 1, i);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}
	gScene->addActor(*gGroundPlane);
}

void PhysicsSystem::cleanupGroundPlane() {
	gGroundPlane->release();
}

void PhysicsSystem::initMaterialFrictionTable() {
	//Each physx material can be mapped to a tire friction value on a per tire basis.
	//If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
	//In this snippet there is only a single material so there can only be a single mapping between material and friction.
	//In this snippet the same mapping is used by all tires.
	gPhysXMaterialFrictions[0].friction = 1.0f;
	gPhysXMaterialFrictions[0].material = gMaterial;
	gPhysXDefaultMaterialFriction = 1.0f;
	gNbPhysXMaterialFrictions = 1;
}
bool PhysicsSystem::initVehicles() {
	using namespace physx;
	using namespace snippetvehicle2;

	// Number of vehicles to create
	const size_t numVehicles = 2;
	vehicles.resize(numVehicles);
	vehicleCommands.resize(numVehicles); // Initialize commands (default to zero)

	// Load parameters once (assuming both vehicles use the same params)
	snippetvehicle2::BaseVehicleParams baseParams;
	snippetvehicle2::PhysXIntegrationParams physxParams;
	EngineDrivetrainParams engineDriveParams;

	readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", baseParams);
	setPhysXIntegrationParams(baseParams.axleDescription,
		gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
		physxParams);
	readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json", engineDriveParams);

	for (size_t i = 0; i < numVehicles; i++) {
		VehicleData& vehicleData = vehicles[i];

		// Set parameters
		vehicleData.vehicle.mBaseParams = baseParams;
		vehicleData.vehicle.mPhysXParams = physxParams;
		vehicleData.vehicle.mEngineDriveParams = engineDriveParams;

		// Initialize the vehicle
		if (!vehicleData.vehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE)) {
			return false;
		}

		// Set unique name and starting pose
		vehicleData.name = "vehicle" + std::to_string(i);
		PxTransform pose;
		if (i == 0) {
			pose = PxTransform(PxVec3(0.f, 0.f, -10.f), PxQuat(PxIdentity)); // First vehicle
		}
		else {
			pose = PxTransform(PxVec3(5.f, 0.f, -10.f), PxQuat(PxIdentity)); // Second vehicle, offset by 5 units
		}
		vehicleData.vehicle.setUpActor(*gScene, pose, vehicleData.name.c_str());

		// Add to rigid dynamic and transform lists
		PxRigidBody* rigidBody = vehicleData.vehicle.mPhysXState.physxActor.rigidBody;
		PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(rigidBody);
		rigidDynamicList.push_back(rigidDynamic);
		transformList.push_back(new Transform());

		// Set collision filter
		PxFilterData vehicleFilter(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
		PxU32 shapes = rigidBody->getNbShapes();
		for (PxU32 j = 0; j < shapes; j++) {
			PxShape* shape = nullptr;
			rigidBody->getShapes(&shape, 1, j);
			if (shape->getGeometry().getType() == PxGeometryType::eBOX) {
				const PxBoxGeometry& box = static_cast<const PxBoxGeometry&>(shape->getGeometry());
				transformList.back()->scale = glm::vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
			}
			shape->setSimulationFilterData(vehicleFilter);
			shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
		}

		// Set initial gear states
		vehicleData.vehicle.mEngineDriveState.gearboxState.currentGear = vehicleData.vehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
		vehicleData.vehicle.mEngineDriveState.gearboxState.targetGear = vehicleData.vehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
		vehicleData.vehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

		// Set initial previous position and direction
		vehicleData.prevPos = pose.p;
		vehicleData.prevDir = pose.q.getBasisVector2();
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

	// Set initial player vehicle state (assuming first vehicle is the player's)
	gState.playerVehicle.curPos = vehicles[0].prevPos;
	gState.playerVehicle.curDir = vehicles[0].prevDir.getNormalized();

	return true;
}

void PhysicsSystem::cleanupVehicles() {
	for (auto& vehicleData : vehicles) {
		vehicleData.vehicle.destroy();
	}
	vehicles.clear();
	vehicleCommands.clear();
}

bool PhysicsSystem::initPhysics() {
	initPhysX();
	initGroundPlane();
	initMaterialFrictionTable();
	if (!initVehicles())
		return false;
	return true;
}

void PhysicsSystem::cleanupPhysics() {
	cleanupVehicles();
	cleanupGroundPlane();
	cleanupPhysX();

	delete gContactReportCallback;
	gContactReportCallback = nullptr;

}

PhysicsSystem::PhysicsSystem(GameState& gameState, Model& tModel) :
	gState(gameState), trailModel(tModel)
{
	initPhysics();
}

PhysicsSystem::~PhysicsSystem() {
	cleanupPhysics();
}

void PhysicsSystem::addItem(MaterialProp material, physx::PxGeometry* geom, physx::PxTransform transform, float density) {
	physx::PxMaterial*pMaterial = gPhysics->createMaterial(material.staticFriction, material.dynamicFriction, material.restitution);

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

	// Actor properties
	physx::PxRigidBodyExt::updateMassAndInertia(*body, density);
	body->setName("obstacle");
	gScene->addActor(*body);

	// Clean up
	shape->release();
	pMaterial = nullptr;
}

void PhysicsSystem::addTrail(float x, float z, float rot) {
	physx::PxMaterial* pMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.0f);

	// create shape
	float height = .5f;
	float width = 0.01f;
	physx::PxBoxGeometry geom(trailStep/2, height,width);
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
	body->setName("trail");

	gState.staticEntities.push_back(Entity("trail", trailModel, new Transform()));

	gState.staticEntities.back().transform->pos = glm::vec3(
		wallTransform.p.x,
		wallTransform.p.y,
		wallTransform.p.z
	);

	gState.staticEntities.back().transform->rot.x = wallTransform.q.x;
	gState.staticEntities.back().transform->rot.y = wallTransform.q.y;
	gState.staticEntities.back().transform->rot.z = wallTransform.q.z;
	gState.staticEntities.back().transform->rot.w = wallTransform.q.w;

	gState.staticEntities.back().transform->scale = glm::vec3(trailStep / 2, height, width);

	gScene->addActor(*body);

	// Clean up
	shape->release();
	pMaterial = nullptr;
}

physx::PxVec3 PhysicsSystem::getPos(int i) {
	physx::PxVec3 position = rigidDynamicList[i]->getGlobalPose().p;
	return position;
}



Transform* PhysicsSystem::getTransformAt(int i) { return transformList[i]; }

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

void PhysicsSystem::updatePhysics(double dt) {
	gScene->simulate(dt);
	gScene->fetchResults(true);

	updateTransforms(gState.dynamicEntities);
}

void PhysicsSystem::stepPhysics(float timestep, Command& command, Command& controllerCommand) {
	using namespace physx;
	using namespace snippetvehicle2;

	// Update commands for the first vehicle (player-controlled)
	vehicleCommands[0].brake = command.brake + controllerCommand.brake;
	vehicleCommands[0].throttle = command.throttle + controllerCommand.throttle;
	vehicleCommands[0].steer = command.steer + controllerCommand.steer;
	// Second vehicle commands remain zero (stationary) by default

	for (size_t i = 0; i < vehicles.size(); i++) {
		VehicleData& vehicleData = vehicles[i];
		EngineDriveVehicle& vehicle = vehicleData.vehicle;

		// Apply commands
		vehicle.mCommandState.brakes[0] = vehicleCommands[i].brake;
		vehicle.mCommandState.nbBrakes = 1;
		vehicle.mCommandState.throttle = vehicleCommands[i].throttle;
		vehicle.mCommandState.steer = vehicleCommands[i].steer;
		vehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

		// Step the vehicle
		const PxVec3 linVel = vehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
		const PxVec3 forwardDir = vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
		const PxReal forwardSpeed = linVel.dot(forwardDir);
		const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
		vehicle.mComponentSequence.setSubsteps(vehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
		vehicle.step(timestep, gVehicleSimulationContext);

		// Update trails
		const PxVec3 currPos = vehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
		PxVec3 travel = currPos - vehicleData.prevPos;
		int steps = travel.magnitude() / trailStep;
		for (int j = 0; j < steps; j++) {
			float ratio = float(j + 1) / float(steps);
			PxVec3 travNorm = ratio * vehicleData.prevDir.getNormalized() + (1 - ratio) * travel.getNormalized();
			PxVec3 placementLoc = vehicleData.prevPos - 1.2f * gState.dynamicEntities.at(i).transform->scale.x * travNorm;
			addTrail(placementLoc.x, placementLoc.z, -atan2(travNorm.z, travNorm.x));
			vehicleData.prevPos += trailStep * travel.getNormalized();
			if (j + 1 == steps) {
				vehicleData.prevDir = travel;
			}
		}

		// Update player vehicle state (only for the first vehicle)
		if (i == 0) {
			gState.playerVehicle.curPos = currPos;
			gState.playerVehicle.curDir = forwardDir.getNormalized();
		}
	}

	// Simulate the entire PhysX scene
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}

void PhysicsSystem::setVehicleCommand(size_t vehicleIndex, const Command& cmd) {
	if (vehicleIndex < vehicleCommands.size()) {
		vehicleCommands[vehicleIndex] = cmd;
	}
}