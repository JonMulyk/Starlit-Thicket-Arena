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
		//std::cout << score.first << std::endl;
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

//bool splitScreenEnabled = false;
//bool splitScreenEnabled4 = true;
std::string GameState::physicsToUiNameConversion(std::string physicsName)
{
	//std::cout << "physics name: " << physicsName << std::endl;
	if (splitScreenEnabled == false && splitScreenEnabled4 == false) {
		if (physicsName == "playerVehicle") { return "player1"; }
		if (physicsName == "vehicle1") { return "ai1"; }
		if (physicsName == "vehicle2") { return "ai2"; }
		if (physicsName == "vehicle3") { return "ai3"; }
	}
	if (splitScreenEnabled == true && splitScreenEnabled4 == false) {
		if (physicsName == "playerVehicle") { return "player1"; }
		if (physicsName == "vehicle1") { return "player2"; }
		if (physicsName == "vehicle2") { return "ai2"; }
		if (physicsName == "vehicle3") { return "ai3"; }
	}
	if (splitScreenEnabled == false && splitScreenEnabled4 == true) {
		if (physicsName == "playerVehicle") { return "player1"; }
		if (physicsName == "vehicle1") { return "player2"; }
		if (physicsName == "vehicle2") { return "player3"; }
		if (physicsName == "vehicle3") { return "player4"; }
	}
	return "";
}

void GameState::addScoreToVehicle(std::string name, uint64_t amount)
{
	std::string uiName = physicsToUiNameConversion(name);

	// check if key does not exist in map 
	if (uiName == "" || (scores.find(uiName) == scores.end()))
	{
		return;
	}
	
	// bounds checking
	if (scores[uiName] > UINT64_MAX - amount)
	{
		scores[uiName] = UINT64_MAX;
		return;
	}

	scores[uiName] += amount;
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

