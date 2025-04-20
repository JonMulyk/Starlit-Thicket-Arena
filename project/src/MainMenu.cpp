#include "MainMenu.h"

//ctsr
MainMenu::MainMenu(Windowing& window, TTF& textRenderer, Controller& controller)
    : window(window), textRenderer(textRenderer), controller(controller),
    startButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    controlsButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    exitButton(0, 0, 0, 0, glm::vec3(0, 0, 0)) {
    compileShaders();
    initializeUIText();
    backgroundRenderer = new BackgroundRenderer("project/assets/background/backgrounMainMenu.jpg", shader); //load background here
}

//dctsr
MainMenu::~MainMenu() {
    delete backgroundRenderer;
    uiText.clear();
    delete shader;
}

//main function to display menu
void MainMenu::displayMenu() {
    /*
    if (!audioInitialized) {
        audio.init();
        audio.startMenuMusic();
        audioInitialized = true;
    }
    */

    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    bool inMenu = true;
    

    //audio.startMenuMusic();
    while (inMenu && !window.shouldClose()) {
        window.clear();
        renderMenu();
        window.swapBuffer();
        glfwPollEvents();
        handleControllerInput(inMenu);
        handleKeyboardInput(inMenu);

        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { //onclick with mous, remove later
            double xpos, ypos;
            glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
            //glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

            if (startButton.isClicked(xpos, ypos)) {
                inMenu = false;
                //audio.stopMusic();
            }
            if (controlsButton.isClicked(xpos, ypos)) {
                ControlsMenu controlsMenu(window, textRenderer, controller, shader);
                controlsMenu.display();
            }
            if (exitButton.isClicked(xpos, ypos)) {
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                //audio.stopMusic();
            }
        }
    }
}

//intialiazations of stuff, mostly buttons tho
void MainMenu::renderMenu() {
    glDisable(GL_DEPTH_TEST);
    backgroundRenderer->draw();
    //glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    startButton = Button(0.4f * windowWidth, 0.4f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    controlsButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    exitButton = Button(0.4f * windowWidth, 0.6f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));

    if (currentSelection == 0) {
        startButton.setColor(glm::vec3(0, 1, 0));
        controlsButton.setColor(glm::vec3(1, 0, 0));
        exitButton.setColor(glm::vec3(1, 0, 0));
    }
    else if (currentSelection == 1) {
        startButton.setColor(glm::vec3(1, 0, 0));
        controlsButton.setColor(glm::vec3(0, 1, 0));
        exitButton.setColor(glm::vec3(1, 0, 0));
    }
    else {
        startButton.setColor(glm::vec3(1, 0, 0));
        controlsButton.setColor(glm::vec3(1, 0, 0));
        exitButton.setColor(glm::vec3(0, 1, 0));
    }

    startButton.draw(shader, windowWidth, windowHeight);
    controlsButton.draw(shader, windowWidth, windowHeight);
    exitButton.draw(shader, windowWidth, windowHeight);

    renderText(uiText);
}

//text
void MainMenu::initializeUIText() {
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
    float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;

    uiText.push_back(Text(startText, windowWidth * 0.5f, windowHeight * 0.55f, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text(controlsText, windowWidth * 0.5f, windowHeight * 0.45f, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text(exitText, windowWidth * 0.5f, windowHeight * 0.35f, baseScale, glm::vec3(1, 1, 1)));
}

void MainMenu::renderText(const std::vector<Text>& renderingText) {
    for (const auto& text : renderingText) {
        textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
    }
}

//remove later, don't really need, just initialize shader in ctsr
void MainMenu::compileShaders() {
    shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
}

//keyboard
void MainMenu::handleKeyboardInput(bool& inMenu) {
    static bool keyUpPressed = false;
    static bool keyDownPressed = false;
    static bool enterPressed = false;

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_RELEASE && keyUpPressed) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        keyUpPressed = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS) {
        keyUpPressed = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_RELEASE && keyDownPressed) {
        currentSelection = (currentSelection + 1) % 3;
        keyDownPressed = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
        keyDownPressed = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && enterPressed) {
        if (currentSelection == 0) {
            inMenu = false;
            //audio.stopMusic();
        }
        else if (currentSelection == 1) {
            ControlsMenu controlsMenu(window, textRenderer, controller, shader);
            controlsMenu.display();
        }
        else if (currentSelection == 2) {
            glfwSetWindowShouldClose(window.getGLFWwindow(), true);
            //audio.stopMusic();
        }
        enterPressed = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        enterPressed = true;
    }
}

//controller
void MainMenu::handleControllerInput(bool& inMenu) {
    static bool dpadUpPressed = false;
    static bool dpadDownPressed = false;
    static bool aButtonPressed = false;

    if (!controller.isConnected()) return;

    if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP) && dpadUpPressed) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        dpadUpPressed = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP)) {
        dpadUpPressed = true;
    }

    if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN) && dpadDownPressed) {
        currentSelection = (currentSelection + 1) % 3;
        dpadDownPressed = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN)) {
        dpadDownPressed = true;
    }

    if (!controller.isButtonPressed(XINPUT_GAMEPAD_A) && aButtonPressed) {
        if (currentSelection == 0) {
            inMenu = false;
            //audio.stopMusic();
        }
        else if (currentSelection == 1) {
            ControlsMenu controlsMenu(window, textRenderer, controller, shader);
            controlsMenu.display();
        }
        else if (currentSelection == 2) {
            glfwSetWindowShouldClose(window.getGLFWwindow(), true);
            //audio.stopMusic();
        }
        aButtonPressed = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
        aButtonPressed = true;
    }
}
