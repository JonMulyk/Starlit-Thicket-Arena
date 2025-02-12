#pragma once
#include <vector>
#include "PxPhysicsAPI.h"
#include "Entity.h"

struct PlayerVehicle {
	physx::PxVec3 curDir;
	physx::PxVec3 prevDir;
	physx::PxVec3 curPos;
	physx::PxVec3 prevPos;
};

class GameState {
public:
	PlayerVehicle playerVehicle;
	std::vector<Entity> dynamicEntities;
	std::vector<Entity> staticEntities;
};