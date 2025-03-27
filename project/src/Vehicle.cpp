#include "Vehicle.h"
using namespace physx;

// Assign the commands
void Vehicle::setPhysxCommand(Command& c) {
	vehicle.mCommandState.brakes[0] = 0;
	vehicle.mCommandState.nbBrakes = 1;
	vehicle.mCommandState.throttle = c.throttle;
	vehicle.mCommandState.steer = c.steer;
	vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
}
void Vehicle::setPhysxCommand() {
	vehicle.mCommandState.brakes[0] = 0;
	vehicle.mCommandState.nbBrakes = 1;
	vehicle.mCommandState.throttle = command.throttle;
	vehicle.mCommandState.steer = command.steer;
	vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
}

// The finite state machine loop
void Vehicle::update(GameState& gState) {
	// reset the command
	command.steer = 0;
	command.throttle = 1;
	command.brake = 0;
	command.boost = false;

	physx::PxVec3 targetLocaton = gState.playerVehicle.curPos + gState.playerVehicle.curDir.getNormalized() * 20.f;
	float targetDist = (targetLocaton - prevPos).magnitude();
	float bufferDist = forwardSearch(gState);

	if (bufferDist < 30) {
		avoid(gState);
	}
	else if (targetDist < 100) {
		if (gState.gMap.castRayTo(prevPos.x, prevPos.z, targetLocaton.x, targetLocaton.z)) {
			directAttack(gState, targetLocaton);
		}
		else if (!plannedAttack(gState, targetLocaton)) {
			wander(gState);
		}
	}
	else {
		wander(gState);
	}

	setPhysxCommand();
}

// check if there is collision up ahead
float Vehicle::forwardSearch(GameState& gState) {
	// initial size very large
	float obstacleDist = 999999999.f;
	float baseAngle = atan2(prevDir.z, prevDir.x);

	// go through all the angles
	for (float i = -5; i <= 5; i += 1.f) {
		float angle = baseAngle + i * PI / 180;
		if (angle < -PI) {
			angle += 2 * PI;
		}
		else if (angle > PI) {
			angle -= 2 * PI;
		}
		float dist = gState.gMap.rayCast(prevPos.x, prevPos.z, angle, 0.1f);

		if (dist < obstacleDist) {
			obstacleDist = dist;
		}
	}
	return obstacleDist;
}

// helpers
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
	if (command.fuel > 0 && abs(command.steer) < .1f) {
		command.boost = true;
	}
	command.brake = 0;
}

bool Vehicle::plannedAttack(GameState& gState, PxVec3 target) {
	findPathToPlayer(gState, target);

	if (path.empty()) {
		return false;
	}
	else {
		commandFromPath();
		return true;
	}
}

void Vehicle::findPathToPlayer(GameState& gState, PxVec3 dst) {
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
	command.brake = 1;
}

bool Vehicle::arcClearance(const PxVec2& initPos, float initAng, int turnDir, GameState& gState, float& dist) {
	// max distance for min calculation
	dist = FLT_MAX;

	// car constraints
	const float radius = 15.0f;
	const float step = 0.2f;
	float omega = 1.0f / radius;

	// track progress
	float arcLen = 0.0;
	PxVec2 pos = initPos;
	float angle = initAng;

	// check clearance along curve
	while (arcLen < radius * PI / 2) {
		// update angle and position
		angle += turnDir * omega * step;
		pos.x += cos(angle) * step;
		pos.y += sin(angle) * step;

		// increment 
		arcLen += step;

		// check for clearance
		if (!gState.gMap.castRayTo(
			pos.x, pos.y, // start
			pos.x + .5f * cos(angle), pos.y + .5f * sin(angle)) // end
		) {
			return false;
		}

		dist = arcLen;
	}
	return true;
}

// wander
float Vehicle::openArea(GameState& gState) {
	// initial size very large
	float obstacleDist = 0.f;
	float baseAngle = atan2(prevDir.z, prevDir.x);
	float dstAngle = 0;

	// go through all the angles
	for (float i = -90; i <= 90; i += 5.f) {
		float angle = baseAngle + i * PI / 180;
		if (angle < -PI) {
			angle += 2 * PI;
		}
		else if (angle > PI) {
			angle -= 2 * PI;
		}
		float distance = gState.gMap.rayCast(prevPos.x, prevPos.z, angle, 0.1f);

		if (distance > obstacleDist) {
			obstacleDist = distance;
			dstAngle = i;
		}
	}

	if (abs(dstAngle) <= 15) {
		return 0;
	}
	else if (dstAngle < 0) {
		return 1;
	}

	return -1;
}

void Vehicle::wander(GameState& gState) {
	command.steer = openArea(gState);
	command.throttle = 1;
	command.brake = 0;
}
