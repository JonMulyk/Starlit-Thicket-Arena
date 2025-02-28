#include "UIManager.h"

UIManager::UIManager(int windowWidth, int windowHeight)
	: windowWidth(windowWidth), windowHeight(windowHeight)
{
    initializeUIText();
}

void UIManager::initializeUIText()
{
    Text uiFPS = Text("", 10.0f, 1410.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
    Text roundTimeLeft = Text("roundTimeLeft", "0:0", static_cast<float>(windowWidth / 2), 1390.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    Text score = Text("Score:  0 : 0", static_cast<float>(windowWidth) - 100.0f, 1390.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

    uiText.push_back(uiFPS);
    uiText.push_back(roundTimeLeft);
    uiText.push_back(score);
}


const std::vector<Text>& UIManager::getUIText() const
{
    return this->uiText;
}

void UIManager::updateUIText(TimeSeconds& timer, double roundDuration)
{
	uiText[0].setTextToRender("FPS: " + std::to_string(timer.getFPS()));
	uiText[1].setTextToRender(timer.formatTimeToHumanReadable(timer.getRemainingTime(roundDuration)));
}

