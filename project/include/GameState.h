#pragma once
#include "GameMap.h"
#include "PxPhysicsAPI.h"

class Entity;

struct PlayerVehicle {
	physx::PxVec3 curDir;
	physx::PxVec3 curPos;
};

enum class GameStateEnum {
	MENU,
	PLAYING,
	RESET
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
		void reset() {
			gMap.resetMap();
			score = 0;
			playerVehicle.curPos = physx::PxVec3(0.0f, 0.0f, 0.0f);
			playerVehicle.curDir = physx::PxVec3(1.0f, 0.0f, 0.0f); 
			//gMap.reset();
		}
};

