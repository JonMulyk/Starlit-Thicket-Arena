#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "Command.h"
#include "GameState.h"

enum class STATE {
	AVOID,
	WANDER,
	DIRECT_ATTACK,
	PLANNED_ATTACK
};

class Vehicle {
public:
	// AI state
	STATE state = STATE::AVOID;
	bool pathPossible = true;					 // Flag for when there is no path to player; failing A* is very expensive

	std::vector<std::shared_ptr<Node>> path = std::vector<std::shared_ptr<Node>>();
	snippetvehicle2::EngineDriveVehicle vehicle; // The PhysX vehicle instance
	physx::PxVec3 prevPos;                       // Previous position for trail calculation
	physx::PxVec3 prevDir;                       // Previous direction for trail calculation
	physx::PxVec3 forward;						 // Current forward vector
	physx::PxVec3 velocity;						 // Current velocity
	std::string name;							 // Unique name for the vehicle actor
	Command command;							 // Vehicle command
	
	// update physx commands using argument
	void setPhysxCommand(Command& c) {
		vehicle.mCommandState.brakes[0] = c.brake;
		vehicle.mCommandState.nbBrakes = 1;
		vehicle.mCommandState.throttle = c.throttle;
		vehicle.mCommandState.steer = c.steer;
		vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
	}

	// update physx command using the vehicle command
	void setPhysxCommand() {
		vehicle.mCommandState.brakes[0] = command.brake;
		vehicle.mCommandState.nbBrakes = 1;
		vehicle.mCommandState.throttle = command.throttle;
		vehicle.mCommandState.steer = command.steer;
		vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
	}

	// find path to player
	void findPathToPlayer(GameState& gState) {
		physx::PxVec3 dst = gState.playerVehicle.curPos + gState.playerVehicle.curDir * 5.f;
		path = gState.gMap.aStar(
			atan2(prevDir.z, prevDir.x),
			prevPos.x, prevPos.z,
			dst.x, dst.z
		);
	}

	// get the command to go to next way point
	bool commandFromPath() {
		// if path is empty dont compute
		if (path.empty()) return false;

		std::shared_ptr<Node> node;
		physx::PxVec2 aiPos(prevPos.x, prevPos.z);

		// select node to target
		for (int i = 0; i < path.size()-1; i++) {
			physx::PxVec2 nodePos(path[i]->xActual, path[i]->yActual);

			// check if node is farther than speed
			if ((nodePos - aiPos).magnitude() > velocity.magnitude() ) {
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

		command.steer = physx::PxClamp(angle, -1.f, 1.f);
		command.throttle = 1;
		command.brake = 0;

		path.erase(path.begin());
		return true;
	}

	void update(GameState& gState) {
		switch (state) {
		case STATE::AVOID:
			avoid(gState);
			break;
		case STATE::WANDER:
			wander(gState);
			break;
		case STATE::DIRECT_ATTACK:
			directAttack(gState);
			break;
		case STATE::PLANNED_ATTACK:
			plannedAttack(gState);
			break;
		}

		setPhysxCommand();
	}

private:
	float steerToPoint(float x, float y) {
		using namespace physx;
		PxVec2 dstPos = { x, y };
		PxVec2 srcPos = { prevPos.x, prevPos.z };
		PxVec2 dstDir = (dstPos - srcPos).getNormalized();
		PxVec2 srcDir = { prevDir.x, prevDir.z };
		srcDir = srcDir.getNormalized();


		// get the angle between forward and node direction
		float dot = srcDir.x * dstDir.x + srcDir.y * dstDir.y;
		float det = srcDir.y * dstDir.x - srcDir.x * dstDir.y;

		return atan2(det, dot);
	}

	// direct path infront of player
	void directAttack(GameState& gState) {
		physx::PxVec3 player = gState.playerVehicle.curDir * 5.f + gState.playerVehicle.curPos;
		if (gState.gMap.castRayTo(prevPos.x, prevPos.z, player.x, player.z)) {
			float angle = steerToPoint(player.x, player.z);

			command.steer = physx::PxClamp(angle, -1.f, 1.f);
			command.throttle = 1;
			command.brake = 0;
		}
	}

	// if there is no direct way, get a path
	void plannedAttack(GameState& gState) {
		using namespace physx;
		if (pathPossible) {
			findPathToPlayer(gState);

			if (path.empty()) {
				pathPossible = false;
			}
			commandFromPath();
		}
		else {
			command.steer = 0;
			command.throttle = 0;
			command.brake = 1.;
		}
	}

	// if there is an obsticle coming up avoid it
	void avoid(GameState& gState) {
		physx::PxVec2 loc = gState.gMap.openArea(prevPos.x, prevPos.z);
		std::cout << loc.x << " " << loc.y << std::endl;
		float angle = steerToPoint(loc.x, loc.y);
		command.steer = physx::PxClamp(angle, -1.f, 1.f);
		command.throttle = 1;
		command.brake = 0;
	}

	// if there no way to the player and there are no obsticles, roam to free space
	void wander(GameState& gState) {

	}
};