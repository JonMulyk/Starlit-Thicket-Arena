#include "LevelSelect.h"

LevelSelectMenu::LevelSelectMenu(Windowing& window, TTF& textRenderer, Controller& controller, GameState& gameState)
    : window(window), textRenderer(textRenderer), controller(controller), gameState(gameState),
    permanentButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    normalButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    backButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    audioInitialized(false) {
    initializeUIText();
    compileShaders();
}

LevelSelectMenu::~LevelSelectMenu() {
    delete backgroundRenderer;
    uiText.clear();
    delete shader;
}

int LevelSelectMenu::displayMenuLevel() {
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (!audioInitialized) {
        audio.init();
        audio.startLevelMusic();
        audioInitialized = true;
    }
    audio.startLevelMusic();

    int selectedLevel = 0;
    backgroundRenderer = new BackgroundRenderer("project/assets/background/levelSelect.jpg", shader);

    while (selectedLevel == 0 && !window.shouldClose()) {
        window.clear();
        renderMenu();
        window.swapBuffer();
        glfwPollEvents();

        handleKeyboardInput(selectedLevel);
        handleControllerInput(selectedLevel);

        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);

            if (permanentButton.isClicked(xpos, ypos)) {
                selectedLevel = 1; // Permanent
                audio.stopMusic();
            }
            else if (normalButton.isClicked(xpos, ypos)) {
                selectedLevel = 2; // Normal
                audio.stopMusic();
            }
            else if (backButton.isClicked(xpos, ypos)) {
                selectedLevel = -1;
                audio.stopMusic();
            }
        }
    }

    return selectedLevel;
}

void LevelSelectMenu::renderMenu() {
    glDisable(GL_DEPTH_TEST);
    backgroundRenderer->draw();
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    float buttonY = 0.85f * windowHeight;
    float buttonHeight = 0.0625f * windowHeight;
    float buttonWidth = 0.2f * windowWidth;
    float spacing = 0.05f * windowWidth;
    float totalWidth = 3 * buttonWidth + 2 * spacing;
    float startX = (windowWidth - totalWidth) / 2;

    permanentButton = Button(startX, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
    normalButton = Button(startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
    backButton = Button(startX + 2 * (buttonWidth + spacing), buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));

    permanentButton.setColor(currentSelection == 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
    normalButton.setColor(currentSelection == 1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
    backButton.setColor(currentSelection == 2 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));

    permanentButton.draw(shader, windowWidth, windowHeight);
    normalButton.draw(shader, windowWidth, windowHeight);
    backButton.draw(shader, windowWidth, windowHeight);

    renderText(uiText);
}

void LevelSelectMenu::initializeUIText() {
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    float buttonHeight = 0.1 * windowHeight;
    float buttonWidth = 0.2f * windowWidth;
    float spacing = 0.05f * windowWidth;
    float totalWidth = 3 * buttonWidth + 2 * spacing;
    float shiftRight = 0.1f * windowWidth;
    float startX = (windowWidth - totalWidth) / 2 + shiftRight;
    float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;

    uiText.clear();
    uiText.push_back(Text("Permanent", startX, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("Normal", startX + buttonWidth + spacing, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("Back", startX + 2 * (buttonWidth + spacing), buttonHeight, baseScale, glm::vec3(1, 1, 1)));
}

void LevelSelectMenu::compileShaders() {
    shader = new Shader("LevelSelectShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
}

void LevelSelectMenu::renderText(const std::vector<Text>& renderingText) {
    for (const auto& text : renderingText) {
        textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
    }
}

void LevelSelectMenu::handleKeyboardInput(int& selectedLevel) {
    static bool keyLeftReleased = false;
    static bool keyRightReleased = false;
    static bool keyEnterReleased = false;

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_RELEASE && keyLeftReleased) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        keyLeftReleased = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
        keyLeftReleased = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_RELEASE && keyRightReleased) {
        currentSelection = (currentSelection + 1) % 3;
        keyRightReleased = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        keyRightReleased = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && keyEnterReleased) {
        if (currentSelection == 2) {
            selectedLevel = -1;
        }
        else {
            selectedLevel = currentSelection + 1;
        }
        audio.stopMusic();
        keyEnterReleased = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        keyEnterReleased = true;
    }
}

void LevelSelectMenu::handleControllerInput(int& selectedLevel) {
    static bool dpadLeftReleased = false;
    static bool dpadRightReleased = false;
    static bool aButtonReleased = false;

    if (!controller.isConnected()) return;

    if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_LEFT) && dpadLeftReleased) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        dpadLeftReleased = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT)) {
        dpadLeftReleased = true;
    }

    if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_RIGHT) && dpadRightReleased) {
        currentSelection = (currentSelection + 1) % 3;
        dpadRightReleased = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) {
        dpadRightReleased = true;
    }

    if (controller.isButtonReleased(XINPUT_GAMEPAD_A) && aButtonReleased) {
        if (currentSelection == 2) {
            selectedLevel = -1;
        }
        else {
            selectedLevel = currentSelection + 1;
        }
        audio.stopMusic();
        aButtonReleased = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
        aButtonReleased = true;
    }
}
