#include "UIManager.h"
#include <sstream>
#include <iomanip>

UIManager::UIManager(int windowWidth, int windowHeight)
	: windowWidth(windowWidth), windowHeight(windowHeight)
{
    initializeUIText();
}

void UIManager::initializeUIText()
{
    Text uiFPS = Text("", 10.0f, 1410.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
    Text roundTimeLeft = Text("roundTimeLeft", "0:0", static_cast<float>(windowWidth / 2), 1390.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    Text fuel = Text("fuel", "100", 0, 1, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    Text score = Text("SCORE", 10.0f, 1370.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

    uiText.push_back(uiFPS);
    uiText.push_back(roundTimeLeft);
    uiText.push_back(fuel);
    uiText.push_back(score);
}

void UIManager::addScoreText(GameState& gameState)
{
    //Clear old score texts
    uiText.erase(uiText.begin() + 4, uiText.end());

    float scoreHeight = 1300.0f;
    float heightIncrementOffset = 50.0f;

    for (const auto& score : gameState.getSortedScores())
    {
        //formatting for the string
		std::ostringstream oss;
        oss << std::left << score.first << ":  " << std::setw(3) << score.second;
        //std::string scoreText = score.first + ":" + std::to_string(score.second);
        std::string scoreText = oss.str();

        // adding text to the vector
        uiText.push_back(Text(scoreText, 10.0f, scoreHeight, 1.0f, glm::vec3(0.5, 0.8f, 0.2f)));
        scoreHeight -= heightIncrementOffset;
    }
}


const std::vector<Text>& UIManager::getUIText() const
{
    return this->uiText;
}

void UIManager::updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState)
{
	uiText[0].setTextToRender("FPS: " + std::to_string(timer.getFPS()));
	uiText[1].setTextToRender(timer.formatTimeToHumanReadable(timer.getRemainingTime(roundDuration)));
    uiText[2].setTextToRender("Fuel:" + std::to_string(int(gameState.playerVehicle.fuel*100)));
    //uiText[2].setTextToRender(gameState.getSortedScoresString());
    //uiText[2].setTextToRender("score: " + std::to_string(score));
    addScoreText(gameState);
}

