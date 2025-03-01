#include "GameState.h"

GameState::GameState()
{
}

void GameState::addDynamicEntity(const std::string name, Model& model, Transform* transform)
{
	this->dynamicEntities.emplace_back(name, model, transform);
}

void GameState::addStaticEntity(const std::string name, Model& model, Transform* transform)
{
	this->staticEntities.emplace_back(name, model, transform);
}

std::vector<Entity*> GameState::findDynamicEntitiesWithName(std::string name)
{
	std::vector<Entity*> matches;
	for (const auto& entity : dynamicEntities)
	{
		if (entity.name == name)
		{
			matches.emplace_back(entity);
		}

	}

	return matches;
}

std::vector<Entity*> GameState::findStaticEntitiesWithName(std::string name)
{
	std::vector<Entity*> matches;
	for (const auto& entity : staticEntities)
	{
		if (entity.name == name)
		{
			matches.emplace_back(entity);
		}

	}

	return matches;
}


void GameState::startRound()
{
}

void GameState::resetRound()
{
}

void GameState::endGame()
{
}
