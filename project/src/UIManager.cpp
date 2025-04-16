#include "UIManager.h"
#include <sstream>
#include <iomanip>

UIManager::UIManager(int windowWidth, int windowHeight, Shader& fuelBarShader)
	: windowWidth(windowWidth), windowHeight(windowHeight), fuelBarShader(fuelBarShader)
{
    this->textScale = std::min(windowWidth, windowHeight) * 0.0010f;
    initializeUIText();
}

void UIManager::initializeUIElements(GameState& gameState)
{
    gameState.initializeScores();
	this->addScoreText(gameState);
	this->initializeFuelBars(gameState.getNumberOfPlayers(), gameState);
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
    Text fuel = Text("fuel", "Fuel:", farLeftX, height*0.026f, this->textScale, textColor, TTF::TEXT_POSITION::LEFT);
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

void UIManager::updateUIText(TimeSeconds& timer, double roundDuration, GameState& gameState, int player)
{
	uiText[0].setTextToRender("FPS: " + std::to_string(timer.getFPS()));
	uiText[1].setTextToRender(timer.formatTimeToHumanReadable(timer.getRemainingTime(roundDuration)));
    if (fuelTextEnabled)
    {
        if (player == 0) uiText[2].setTextToRender("Fuel:" + std::to_string(int(gameState.playerVehicle.fuel * 100)));
        else if (player == 1) uiText[2].setTextToRender("Fuel:" + std::to_string(int(gameState.playerVehicle2.fuel * 100)));
        else if (player == 2) uiText[2].setTextToRender("Fuel:" + std::to_string(int(gameState.playerVehicle3.fuel * 100)));
        else if (player == 3) uiText[2].setTextToRender("Fuel:" + std::to_string(int(gameState.playerVehicle4.fuel * 100)));
    }

    addScoreText(gameState);
}

void UIManager::initializeFuelBars(int numberOfBars, GameState& gameState)
{
    fuelBars.clear();

    for (int i = 0; i < numberOfBars; i++)
    {
        this->fuelBars.emplace_back(this->fuelBarShader, -0.9f, -0.95f, 0.4f, 0.05f);
    }

    this->resetFuel(gameState);
}

std::vector<FuelBar>& UIManager::getFuelBars()
{
    return this->fuelBars;
}

void UIManager::resetFuel(GameState& gameState)
{
    gameState.playerVehicle.fuel  = 1.0f;
    gameState.playerVehicle2.fuel = 1.0f;
    gameState.playerVehicle3.fuel = 1.0f;
    gameState.playerVehicle4.fuel = 1.0f;
}

