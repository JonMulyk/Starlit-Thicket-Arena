#include "EndScreen.h"
#include "MainMenu.h"

//ctsr
EndScreen::EndScreen(Windowing& window, TTF& textRenderer, Controller& controller, AudioSystem& audio, GameState& gameState)
    : window(window), textRenderer(textRenderer), controller(controller), audio(audio),  gameState(gameState),
    startButton(0, 0, 0, 0, glm::vec3(0, 0, 0)) {
    compileShaders();
}

//dctsr
EndScreen::~EndScreen() {
    delete backgroundRenderer;
    uiText.clear();
    delete shader;
}

//main function to display menu
void EndScreen::displayMenu() {
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    uiText.clear();
    initializeUIText();
    bool inMenu = true;
    backgroundRenderer = new BackgroundRenderer("project/assets/background/backgroundEndScreen.jpg", shader);

    while (inMenu && !window.shouldClose()) {
        window.clear();
        renderMenu();
        window.swapBuffer();
        glfwPollEvents();
        handleControllerInput(inMenu);
        handleKeyboardInput(inMenu);

        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
            glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

            if (startButton.isClicked(xpos, ypos)) {
                inMenu = false;
                audio.stopMusic();
            }
        }
    }
}

//intialiazations of stuff, mostly buttons tho
void EndScreen::renderMenu() {
    glDisable(GL_DEPTH_TEST);
    backgroundRenderer->draw();
    boxRenderer.draw(-0.6f, -0.6f, 1.2f, 1.2f, 0.5f);
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    startButton = Button(0.4f * windowWidth, (1-0.15f) * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));

    if (currentSelection == 0) {
        startButton.setColor(glm::vec3(0, 1, 0));
    }

    startButton.draw(shader, windowWidth, windowHeight);
    renderText(uiText);
}

//text
void EndScreen::initializeUIText() {
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
    float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;

    uiText.push_back(Text("Game Over", windowWidth * 0.5f, windowHeight * 0.90f, baseScale, glm::vec3(1, 1, 1)));

    uiText.push_back(Text("Main Menu", windowWidth * 0.5f, windowHeight * 0.1f, baseScale, glm::vec3(1, 1, 1)));

    uiText.push_back(Text("Scores", windowWidth * 0.5f, windowHeight * 0.75f, baseScale, glm::vec3(1, 1, 1)));


    float scoreHeight = static_cast<float>(windowHeight) * 0.6f;

    float heightIncrementOffset = static_cast<float>(windowHeight) * 0.08; 

    float nameXPos = windowWidth * 0.25f; 

    float scoreXPos = windowWidth * 0.75f; 

    for (const auto& score : gameState.getSortedScores()) {
 
        std::ostringstream oss;
        oss << std::left << std::setw(20) << score.first + ":" << "  " << score.second;
        std::string scoreText = oss.str();

        float centerX = windowWidth * 0.5f; 

        uiText.push_back(Text(score.first, nameXPos, scoreHeight, this->textScale * 0.8f, glm::vec3(0.5f, 0.8f, 0.2f), TTF::TEXT_POSITION::LEFT));


        uiText.push_back(Text(std::to_string(score.second), scoreXPos, scoreHeight, this->textScale * 0.8f, glm::vec3(0.5f, 0.8f, 0.2f), TTF::TEXT_POSITION::RIGHT));

        scoreHeight -= heightIncrementOffset;
    }
}



void EndScreen::renderText(const std::vector<Text>& renderingText) {
    for (const auto& text : renderingText) {
        textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
    }
}

//remove later, don't really need, just initialize shader in ctsr
void EndScreen::compileShaders() {
    shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
}

//keyboard
void EndScreen::handleKeyboardInput(bool& inMenu) {
    static bool enterPressed = false;

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && enterPressed) {
        inMenu = false;
        audio.stopMusic();
        enterPressed = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        enterPressed = true;
    }
}

//controller
void EndScreen::handleControllerInput(bool& inMenu) {
    static bool aButtonPressed = false;

    if (!controller.isConnected()) return;

    if (!controller.isButtonPressed(XINPUT_GAMEPAD_A) && aButtonPressed) {
        inMenu = false;
        audio.stopMusic();
        aButtonPressed = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
        aButtonPressed = true;
    }
}
