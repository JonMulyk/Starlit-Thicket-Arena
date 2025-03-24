#include "GameState.h"
#include <string>


GameState::GameState()
{
}

std::vector<std::pair<std::string, uint64_t>> GameState::getSortedScores()
{
	return this->sortedScores;
}

std::string GameState::getSortedScoresString()
{
	std::string scoresString;
	for (const auto& score : sortedScores)
	{
		scoresString += (score.first + ": " + std::to_string(score.second) + "\n");
	}

	if(scoresString[scoresString.size() - 1] == '\n')
	{ 
		scoresString.pop_back();
	}
	
	return scoresString;
}

// non-member helper function
bool compareByScore(const std::pair<std::string, uint64_t>& a, const std::pair<std::string, uint64_t>& b)
{
	return a.second > b.second;
}

void GameState::sortScores()
{
	std::vector<std::pair<std::string, uint64_t>> scoresVec(scores.begin(), scores.end());

	std::sort(scoresVec.begin(), scoresVec.end(), compareByScore);

	sortedScores = std::move(scoresVec);
}

void GameState::addScoreToVehicle(std::string name, uint64_t amount)
{
	if (scores[name] > UINT64_MAX - amount)
	{
		scores[name] = UINT64_MAX;
		return;
		//throw std::runtime_error("Error: score will overflow, score: " + std::to_string(score));
	}

	scores[name] += amount;
	sortScores();
}

void GameState::initializeScores(uint16_t numberOfPlayers, uint16_t numberOfAiCars)
{
	scores.clear();

	for (unsigned int i = 1; i < numberOfPlayers+1; i++)
	{
		scores.emplace("player" + std::to_string(i), 0);
	}

	for (unsigned int i = 1; i < numberOfAiCars+1; i++)
	{
		scores.emplace("ai" + std::to_string(i), 0);
	}

	sortScores();
}

