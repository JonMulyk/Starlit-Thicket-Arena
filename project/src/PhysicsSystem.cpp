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

	ContactReportCallback* gContactReportCallback = new ContactReportCallback();
	sceneDesc.simulationEventCallback = gContactReportCallback;

	gScene = gPhysics->createScene(sceneDesc);
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

	//Load the params from json or set directly.
	readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle.mBaseParams);
	setPhysXIntegrationParams(gVehicle.mBaseParams.axleDescription,
		gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
		gVehicle.mPhysXParams);
	readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json",
		gVehicle.mEngineDriveParams);

	//Set the states to default.
	if (!gVehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
	{
		return false;
	}

	//Apply a start pose to the physx actor and add it to the physx scene.
	PxTransform pose(PxVec3(0.f, -0.f, -10.f), PxQuat(PxIdentity));
	gVehicle.setUpActor(*gScene, pose, gVehicleName);
	PxRigidBody* rigidBody = gVehicle.mPhysXState.physxActor.rigidBody;
	PxRigidDynamic* rigidDynamic = (PxRigidDynamic*)rigidBody;

	rigidDynamicList.push_back(rigidDynamic);
	transformList.push_back(new Transform());

	// Create filter
	PxFilterData vehicleFilter(
		COLLISION_FLAG_CHASSIS,
		COLLISION_FLAG_CHASSIS_AGAINST,
		0, 0
	);

	PxU32 shapes = gVehicle.mPhysXState.physxActor.rigidBody->getNbShapes();
	for (PxU32 i = 0; i < shapes; i++) {
		PxShape* shape = NULL;
		gVehicle.mPhysXState.physxActor.rigidBody->getShapes(&shape, 1, i);

		if (shape->getGeometry().getType() == PxGeometryType::eBOX) {
			const PxBoxGeometry& box = static_cast<const PxBoxGeometry&>(shape->getGeometry());
			transformList.back()->scale = glm::vec3(box.halfExtents.x, box.halfExtents.y, box.halfExtents.z);
		}

		shape->setSimulationFilterData(vehicleFilter);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	//Set the vehicle in 1st gear.
	gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
	gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;

	//Set the vehicle to use the automatic gearbox.
	gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//Set up the simulation context.
	//The snippet is set up with
	//a) z as the longitudinal axis
	//b) x as the lateral axis
	//c) y as the vertical axis.
	//d) metres  as the lengthscale.
	gVehicleSimulationContext.setToDefault();
	gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
	gVehicleSimulationContext.scale.scale = 1.0f;
	gVehicleSimulationContext.gravity = gGravity;
	gVehicleSimulationContext.physxScene = gScene;
	gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;

	// set the spaw location
	vehiclePrevPos = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	vehiclePrevDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();

	// set the vehicle state
	gState.playerVehicle.curDir = vehiclePrevDir.getNormalized();
	gState.playerVehicle.curPos = vehiclePrevPos;

	return true;
}

void PhysicsSystem::cleanupVehicles() {
	gVehicle.destroy();
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


	physx::PxRigidBodyExt::updateMassAndInertia(*body, density);
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

void PhysicsSystem::stepPhysics(float timestep, Command& command, Command& controllerCommand, bool& audioMove) {
	using namespace physx;
	using namespace snippetvehicle2;

	//Apply the brake, throttle and steer to the command state of the vehicle.
	gVehicle.mCommandState.brakes[0] = command.brake + controllerCommand.brake;
	gVehicle.mCommandState.nbBrakes = 1;
	gVehicle.mCommandState.throttle = command.throttle + controllerCommand.throttle;
	gVehicle.mCommandState.steer = command.steer + controllerCommand.steer;
	// gVehicle.mTransmissionCommandState.targetGear = physx::vehicle2::PxVehicleDirectDriveTransmissionCommandState::eREVERSE;
	gVehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//Forward integrate the vehicle by a single timestep.
	const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
	const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
	const PxReal forwardSpeed = linVel.dot(forwardDir);
	const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);

	gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
	gVehicle.step(timestep, gVehicleSimulationContext);

	const physx::PxVec3 currPos = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;

	// Update after timestep
	gState.playerVehicle.curPos = currPos;
	gState.playerVehicle.curDir = forwardDir.getNormalized();

	physx::PxVec3 travel = currPos - vehiclePrevPos;

	int steps = travel.magnitude() / trailStep;
	if (steps > 0) audioMove = true;

	for (int i=0; i < steps; i++) {
		float ratio = float(i + 1) / float(steps);

		physx::PxVec3 travNorm = ratio*vehiclePrevDir.getNormalized() + (1-ratio)*travel.getNormalized();
		physx::PxVec3 placementLoc = vehiclePrevPos - 1.2f * gState.dynamicEntities.at(0).transform->scale.x * travNorm;
		addTrail(placementLoc.x, placementLoc.z, -atan2(travNorm.z, travNorm.x));

		vehiclePrevPos += trailStep*travel.getNormalized();

		if (i + 1 == steps) {
			vehiclePrevDir = travel;
		}
	}

	//Forward integrate the phsyx scene by a single timestep.
	gScene->simulate(timestep);
	gScene->fetchResults(true);
}
