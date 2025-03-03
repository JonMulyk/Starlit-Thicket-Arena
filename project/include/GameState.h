#pragma once
#include "GameMap.h"
#include "PxPhysicsAPI.h"
#include "Entity.h"


struct PlayerVehicle {
	physx::PxVec3 curDir;
	physx::PxVec3 curPos;
};

class GameState {
	private:

		uint64_t score = 0;

	public:
		GameMap gMap;
		PlayerVehicle playerVehicle;
		std::vector<Entity> dynamicEntities;
		std::vector<Entity> staticEntities;

		uint64_t getScore();
		void incrementScore();
		void addToScore(uint64_t amount);

};

