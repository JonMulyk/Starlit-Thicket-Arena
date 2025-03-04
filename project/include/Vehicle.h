#pragma once

#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
// #include "snippetvehicle2common/serialization/BaseSerialization.h"
// #include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
// #include "snippetvehicle2common/SnippetVehicleHelpers.h"
// #include "snippetcommon/SnippetPVD.h"

#include "Command.h"

// forward declare gamestate
class GameState;

enum class STATE {
	ATTACK,
	WANDER,
	RECOVER
};

class Vehicle {
private:
	void attack(GameState& gState) {

	}

	void wander(GameState& gState) {

	}

	void recover(GameState& gState) {

	}

public:
	STATE state = STATE::ATTACK;
	snippetvehicle2::EngineDriveVehicle vehicle; // The PhysX vehicle instance
	physx::PxVec3 prevPos;                       // Previous position for trail calculation
	physx::PxVec3 prevDir;                       // Previous direction for trail calculation
	std::string name;							 // Unique name for the vehicle actor
	Command command;

	void updateCommand(Command& c) {
		vehicle.mCommandState.brakes[0] = c.brake;
		vehicle.mCommandState.nbBrakes = 1;
		vehicle.mCommandState.throttle = c.throttle;
		vehicle.mCommandState.steer = c.steer;
		vehicle.mTransmissionCommandState.targetGear = snippetvehicle2::PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
	}

	void update(GameState& gState) {
		switch (state) {
		case STATE::ATTACK:
			attack(gState);
			break;
		case STATE::WANDER:
			wander(gState);
			break;
		case STATE::RECOVER:
			recover(gState);
			break;
		}
	}
};