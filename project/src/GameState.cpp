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


void GameState::startRound()
{
}

void GameState::resetRound()
{
}

void GameState::endGame()
{
}
