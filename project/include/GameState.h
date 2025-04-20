#pragma once
#include "GameMap.h"
#include "Entity.h"
#include "PxPhysicsAPI.h"
#include <vector>
#include <iostream>
#include "Transform.h"

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
	RESET,
	PAUSE
};

class GameState {
	private:
		std::unordered_map<std::string, uint64_t> scores;
		std::unordered_map <std::string, glm::vec3 > scoreToColor;
		std::vector<std::pair<std::string, uint64_t>> sortedScores;
		std::vector<glm::vec3> colors = {
		{ 32.0f / 255.0f, 237.0f / 255.0f, 34.0f / 255.0f}, // green
		{ 49.0f / 255.0f, 57.0f / 255.0f, 209.0f / 255.0f}, // blue
		{ 255.0f / 255.0f, 21.0f / 255.0f, 15.0f / 255.0f}, // red
		{ 226.0f / 255.0f, 182.0f / 255.0f, 26.0f / 255.0f} // yellow
		};

	public:
		GameState();

		GameMap gMap;
		PlayerVehicle playerVehicle;
		PlayerVehicle playerVehicle2;
		PlayerVehicle playerVehicle3;
		PlayerVehicle playerVehicle4;
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
		void initializeScores();

		glm::vec3 getColorForScoreName(std::string name);

		bool resetAudio = false;
		bool tempTrails = false;
		bool splitScreenEnabled = false;
		bool splitScreenEnabled4 = false;

		uint16_t getNumberOfPlayers();
		uint16_t getNumberOfAI();
};

