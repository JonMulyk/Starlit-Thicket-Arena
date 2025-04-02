#include "UIManager.h"
#include <sstream>
#include <iomanip>

UIManager::UIManager(int windowWidth, int windowHeight)
	: windowWidth(windowWidth), windowHeight(windowHeight)
{
    this->textScale = std::min(windowWidth, windowHeight) * 0.0010f;
    initializeUIText();
}

void UIManager::initializeUIText()
{

    float width = static_cast<float>(windowWidth);
    float height = static_cast<float>(windowHeight);
    float topOfScreenY = height - (height * 0.045f);
    float farLeftX = width * 0.0055f;

    glm::vec3 textColor = glm::vec3(0.5f, 0.8f, 0.2f);

    Text uiFPS = Text("", farLeftX, topOfScreenY, 0.5f, textColor, TTF::TEXT_POSITION::LEFT);
    Text roundTimeLeft = Text("roundTimeLeft", "0:0", width / 2.0f, topOfScreenY, this->textScale, textColor, TTF::TEXT_POSITION::CENTER);
    Text fuel = Text("fuel", "100", farLeftX, height*0.01f, this->textScale, textColor, TTF::TEXT_POSITION::LEFT);
    Text score = Text("SCORE", farLeftX, height * 0.858f, this->textScale, textColor, TTF::TEXT_POSITION::LEFT);

    uiText.push_back(uiFPS);
    uiText.push_back(roundTimeLeft);
    uiText.push_back(fuel);
    uiText.push_back(score);
}

void UIManager::addScoreText(GameState& gameState)
{
    //Clear old score texts
    uiText.erase(uiText.begin() + 4, uiText.end());

    //float scoreHeight = 1300.0f;
    float scoreHeight = static_cast<float>(windowHeight) * 0.8085f;

    float heightIncrementOffset = static_cast<float>(windowHeight) * 0.0417f;

    float farLeftX = static_cast<float>(windowWidth) * 0.055f;

    for (const auto& score : gameState.getSortedScores())
    {
        //formatting for the string
		std::ostringstream oss;
        oss << std::left << score.first << ":  " << std::setw(3) << score.second;
        //std::string scoreText = score.first + ":" + std::to_string(score.second);
        std::string scoreText = oss.str();

		float farLeftX = static_cast<float>(windowWidth) * 0.0055f;

        // adding text to the vector
        uiText.push_back(Text(scoreText, farLeftX, scoreHeight, this->textScale * 0.8f, glm::vec3(0.5, 0.8f, 0.2f), TTF::TEXT_POSITION::LEFT));
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

