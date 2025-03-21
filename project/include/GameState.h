#pragma once
#include "GameMap.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include <iostream>
#include "Transform.h"

class Entity;

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

		// In GameState.h:
		Transform* FollowTarget1 = nullptr;
		Transform* FollowTarget2 = nullptr;
		Transform* FollowTarget3 = nullptr;

		bool splitScreenEnabled = false;
		bool splitScreenEnabled4 = true;
};

