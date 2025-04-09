#pragma once
#include "GameMap.h"
#include "Entity.h"
#include "PxPhysicsAPI.h"

#include <unordered_map>

class Entity;

struct PlayerVehicle {
	float fuel;
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

		std::unordered_map<std::string, uint64_t> scores;
		std::vector<std::pair<std::string, uint64_t>> sortedScores;

	public:
		GameState();

		GameMap gMap;
		PlayerVehicle playerVehicle;
		std::vector<Entity> dynamicEntities;
		std::vector<Entity> staticEntities;
		
		void reset() {
			gMap.resetMap();
			playerVehicle.curPos = physx::PxVec3(0.0f, 0.0f, 0.0f);
			playerVehicle.curDir = physx::PxVec3(1.0f, 0.0f, 0.0f); 
			//gMap.reset();
		}

		//score	
		std::vector<std::pair<std::string, uint64_t>> getSortedScores();
		std::string getSortedScoresString();
		void sortScores();
		std::string physicsToUiNameConversion(std::string physicsName);
		void addScoreToVehicle(std::string name, uint64_t value);
		void initializeScores(uint16_t numberOfPlayers, uint16_t numberOfAiCars);

		bool resetAudio = false;
		bool tempTrails = true;
};

