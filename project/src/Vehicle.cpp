#include "Vehicle.h"
using namespace physx;

void Vehicle::setPhysxCommand(Command& c) {
	vehicle.mCommandState.brakes[0] = c.brake;
	vehicle.mCommandState.nbBrakes = 1;
	vehicle.mCommandState.throttle = c.throttle;
	vehicle.mCommandState.steer = c.steer;
	vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
}

void Vehicle::setPhysxCommand() {
	vehicle.mCommandState.brakes[0] = command.brake;
	vehicle.mCommandState.nbBrakes = 1;
	vehicle.mCommandState.throttle = command.throttle;
	vehicle.mCommandState.steer = command.steer;
	vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
}

// The finite state machine
void Vehicle::update(GameState& gState) {
	// get distance from object infront
	float obstacleDist = gState.gMap.rayCast(prevPos.x, prevPos.z, atan2(prevDir.z, prevDir.x), 0.1f);
	float targetDist = (gState.playerVehicle.curPos + gState.playerVehicle.curDir.getNormalized() * 15.f - prevPos).magnitude();
	float playerDist = (gState.playerVehicle.curPos - prevPos).magnitude();

	if (obstacleDist < 30) {
		avoid(gState);
	}
	else if (playerDist < 75 || targetDist < 75) {
		if (!plannedAttack(gState)) {
			wander(gState);
		}
	}
	else {
		wander(gState);
	}

	setPhysxCommand();
}

// helpers
float Vehicle::randomSteer() {
	static std::mt19937 gen;
	static std::uniform_real_distribution<float> dist(-1.f, 1.f);
	return dist(gen);
}

float Vehicle::steerToPoint(float x, float y) {
	using namespace physx;
	PxVec2 dstPos = { x, y };
	PxVec2 srcPos = { prevPos.x, prevPos.z };
	PxVec2 dstDir = (dstPos - srcPos).getNormalized();
	PxVec2 srcDir = { prevDir.x, prevDir.z };
	srcDir = srcDir.getNormalized();

	// get the angle between forward and node direction
	float dot = srcDir.x * dstDir.x + srcDir.y * dstDir.y;
	if (sqrt(dot) > 0.95f) {
		return 0;
	}

	float det = srcDir.y * dstDir.x - srcDir.x * dstDir.y;

	if (det < 0) {
		return -1;
	}
	else {
		return 1;
	}
}

// attack player
void Vehicle::directAttack(GameState& gState, PxVec3 player) {
	float angle = steerToPoint(player.x, player.z);
	command.steer = PxClamp(angle / 0.5f, -1.f, 1.f);
	command.throttle = 1;
	command.brake = 0;
}

bool Vehicle::plannedAttack(GameState& gState) {
	findPathToPlayer(gState);

	if (path.empty()) {
		return false;
	}
	else {
		commandFromPath();
		return true;
	}
}

void Vehicle::findPathToPlayer(GameState& gState) {
	PxVec3 dst = gState.playerVehicle.curPos + gState.playerVehicle.curDir.getNormalized() * 20.f;
	path = gState.gMap.aStar(
		atan2(prevDir.z, prevDir.x),
		prevPos.x, prevPos.z,
		dst.x, dst.z
	);
}

bool Vehicle::commandFromPath() {
	// if path is empty dont compute
	if (path.empty()) return false;

	std::shared_ptr<Node> node;
	PxVec2 aiPos(prevPos.x, prevPos.z);

	// select node to target
	for (int i = 0; i < path.size() - 1; i++) {
		PxVec2 nodePos(path[i]->xActual, path[i]->yActual);

		// check if node is farther than speed
		if ((nodePos - aiPos).magnitude() > velocity.magnitude()) {
			node = path[i];
			break;
		}
		else {
			path.erase(path.begin());
		}
	}

	// if node is null set to farthest
	if (!node) {
		node = path.back();
	}

	float angle = steerToPoint(node->xActual, node->yActual);
	command.steer = angle;
	command.throttle = 1;
	command.brake = 0;

	path.erase(path.begin());
	return true;
}

// avoid obstacle
void Vehicle::avoid(GameState& state) {
	PxVec2 pos = { prevPos.x, prevPos.z };
	float ang = atan2(prevDir.z, prevDir.x);

	// Simulate candidate arcs to the left and right.
	float lDist, rDist;
	bool leftOk = arcClearance(pos, ang, 1, state, lDist);
	bool rightOk = arcClearance(pos, ang, -1, state, rDist);

	// update steering
	if (leftOk && !rightOk)
		command.steer = -1;
	else if (rightOk && !leftOk)
		command.steer = 1;
	else
		command.steer = (lDist > rDist) ? -1 : 1;

	command.throttle = 1;
	command.brake = 0;
}

bool Vehicle::arcClearance(const PxVec2& initPos, float initAng, int turnDir, GameState& gState, float& dist) {
	// max distance for min calculation
	dist = FLT_MAX;

	// car constraints
	const float radius = 10.0f;
	const float step = 0.2f;
	float omega = 1.0f / radius;

	// track progress
	float arcLen = 0.0;
	PxVec2 pos = initPos;
	float angle = initAng;

	// check clearance along curve
	while (arcLen < 20.0f) {
		// update angle and position
		angle += turnDir * omega * step;
		pos.x += cos(angle) * step;
		pos.y += sin(angle) * step;

		// increment 
		arcLen += step;

		// get current ray clearance
		float clearance = gState.gMap.rayCast(pos.x, pos.y, angle, 0.1f);
		dist = std::min(dist, clearance);

		// path doesnt have sufficient clearance
		if (clearance < 1.0f)
			return false;
	}
	return true;
}

// wander
void Vehicle::wander(GameState& gState) {
	command.steer = .5f * randomSteer() + .5f * command.steer;
	command.brake = 0;
	command.throttle = 1;
}
