#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "Command.h"
#include "GameState.h"
#include "PhysicsSystem.h"
#include <random>

using namespace physx;

// forward declartion
class PhysicsSystem;

class Vehicle {
public:
	snippetvehicle2::EngineDriveVehicle vehicle;	// The PhysX vehicle instance
	std::vector<std::shared_ptr<Node>> path = std::vector<std::shared_ptr<Node>>();
	PxVec3 prevPos;		// Previous position for trail calculation
	PxVec3 prevDir;     // Previous direction for trail calculation
	PxVec3 forward;		// Current forward vector
	PxVec3 velocity;	// Current velocity
	std::string name;	// Unique name for the vehicle actor
	Command command;	// Vehicle command

	std::uniform_real_distribution<float> dist;

	// update physx commands
	void setPhysxCommand(Command& c);
	void setPhysxCommand();

	// The finite state machine
	void update(GameState& gState, PhysicsSystem& physSys);

private:
	// -- helpers
	const float PI = 3.14159265f;
	float steerToPoint(float x, float y);
	float forwardSearch(GameState& gState);
	void nearestPlayer(GameState& gState, PhysicsSystem& physSys, physx::PxVec3& loc, float& dist);

	// -- direct attack using linear direction
	void directAttack(GameState& gState, PxVec3 player);

	// -- Planned attack using A*
	bool plannedAttack(GameState& gState, PxVec3 target);

	// find path to player
	void findPathToPlayer(GameState& gState, PxVec3 target);

	// get the command to go to next way point
	bool commandFromPath();

	// avoid upcomming obstacles
	void avoid(GameState& state);

	// get the clearance of a turn
	bool arcClearance(const PxVec2& initPos, float initAng, int turnDir, GameState& gState, float& dist);

	// -- wandering
	float openArea(GameState& gState);
	void wander(GameState& gState);
};
