#include "GameState.h"


uint64_t GameState::getScore()
{
	return this->score;
}

void GameState::incrementScore()
{
	if (score == UINT64_MAX)
	{
		return;
		//throw std::runtime_error("Error: score will overflow if incremented, score: " + std::to_string(score));
	}
	this->score++;
}

void GameState::addToScore(uint64_t amount)
{
	if (score > UINT64_MAX - amount)
	{
		score = UINT64_MAX;
		return;
		//throw std::runtime_error("Error: score will overflow, score: " + std::to_string(score));
	}

	score += amount;
}
