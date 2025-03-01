#pragma once
#include <vector>
#include "PxPhysicsAPI.h"
#include "Entity.h"

struct PlayerVehicle {
	physx::PxVec3 curDir;
	physx::PxVec3 curPos;
};

class GameState 
{
	public:
		// constructor
		GameState();

		// public functions
		void addDynamicEntity(const std::string name, Model& model, Transform* transform);
		void addStaticEntity(const std::string name, Model& model, Transform* transform);
		std::vector<Entity*> findDynamicEntitiesWithName(std::string name);
		std::vector<Entity*> findStaticEntitiesWithName(std::string name);

		
		void startRound();
		void resetRound();
		void endGame();

		// Flags
		bool inRound = true;
		bool inMenu = false;
		bool loading = false;
		bool quit = false;
		bool gameEnded = false;

		// game Parameters
		uint8_t numPlayers = 1;
		uint8_t numVehicles = 1;
	
		// game state data
		PlayerVehicle playerVehicle;
		std::vector<Entity> dynamicEntities;
		std::vector<Entity> staticEntities;
	
		// scoring and round data
		uint8_t round = 0;
		uint8_t score = 0;
};