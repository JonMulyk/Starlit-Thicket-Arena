#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include "Controller.h" 
#include <ControlMenu.h>
//#include "BackgroundRenderer.h"
#include <BackgroundRenderer.h>


/*
TODO: organize to seperate from cpp and .h file, essentially optimize and clean up after testing

currently frontloaded in .h for functionality sakes (none of this code is really reusable)
*/
class MainMenu {
public:
    MainMenu(Windowing& window, TTF& textRenderer, Controller& controller)
        : window(window), textRenderer(textRenderer), controller(controller),
        startButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        controlsButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        exitButton(0, 0, 0, 0, glm::vec3(0, 0, 0))
    {
        compileShaders();
        initializeUIText();
        //initializeButtons(); 
    }

    ~MainMenu() {
        delete backgroundRenderer;
        uiText.clear();
        delete shader;
    }


    void displayMenu() {
        if (!audioInitialized) {
            audio.init();
            audio.startMenuMusic();
            audioInitialized = true;
        }
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        bool inMenu = true;
        backgroundRenderer = new BackgroundRenderer("project/assets/background/backgrounMainMenu.jpg", shader);

        audio.startMenuMusic();  
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
                if (controlsButton.isClicked(xpos, ypos)) {
                    ControlsMenu controlsMenu(window, textRenderer, controller, shader);
                    controlsMenu.display();
                }
                if (exitButton.isClicked(xpos, ypos)) {
                    glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                    audio.stopMusic();
                }
            }
        }
    }

private:
    Controller& controller;
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    BackgroundRenderer* backgroundRenderer;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button startButton, controlsButton, exitButton;
    int currentSelection;   
    AudioSystem audio;
    bool audioInitialized;

    const std::string startText = "Start";
    const std::string controlsText = "Controls";
    const std::string exitText = "Exit";

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        //drawBackground();
        backgroundRenderer->draw();

        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        startButton = Button(0.4f * windowWidth, 0.4f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
        controlsButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
        exitButton = Button(0.4f * windowWidth, 0.6f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));

        if (currentSelection == 0) {
            startButton.setColor(glm::vec3(0, 1, 0));     // Green for selected
            controlsButton.setColor(glm::vec3(1, 0, 0));  // Red for unselected
            exitButton.setColor(glm::vec3(1, 0, 0));
        }
        else if (currentSelection == 1) {
            startButton.setColor(glm::vec3(1, 0, 0));
            controlsButton.setColor(glm::vec3(0, 1, 0));  // Green for selected
            exitButton.setColor(glm::vec3(1, 0, 0));
        }
        else { // currentSelection == 2
            startButton.setColor(glm::vec3(1, 0, 0));
            controlsButton.setColor(glm::vec3(1, 0, 0));
            exitButton.setColor(glm::vec3(0, 1, 0));      // Green for selected
        }

        startButton.draw(shader, windowWidth, windowHeight);
        controlsButton.draw(shader, windowWidth, windowHeight);
        exitButton.draw(shader, windowWidth, windowHeight);

        renderText(uiText);
    }


    void initializeUIText() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;

        Text Start = Text(startText, windowWidth * 0.5f, windowHeight * 0.55f, baseScale, glm::vec3(1, 1, 1));
        Text Controls = Text(controlsText, windowWidth * 0.5f, windowHeight * 0.45f, baseScale, glm::vec3(1, 1, 1));
        Text Exit = Text(exitText, windowWidth * 0.5f, windowHeight * 0.35, baseScale, glm::vec3(1, 1, 1));

        uiText.push_back(Start);
        uiText.push_back(Controls);
        uiText.push_back(Exit);
    }

    //void initializeButtons() {
    //    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
     //   startButton = Button(0.4f * windowWidth, 0.4f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    //    controlsButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    //    exitButton = Button(0.4f * windowWidth, 0.6f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    //}

    void renderText(const std::vector<Text>& renderingText) {
        for (const auto& text : renderingText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }

    void compileShaders() {
        //shader class to load shaders
        shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
    }


    void handleKeyboardInput(bool& inMenu) {
        static bool keyUpPressed = false;
        static bool keyDownPressed = false;
        static bool enterPressed = false;

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_RELEASE && keyUpPressed) {
            currentSelection = (currentSelection - 1 + 3) % 3; // Cycle: 2 -> 1 -> 0
            keyUpPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            keyUpPressed = true;
        }

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_RELEASE && keyDownPressed) {
            currentSelection = (currentSelection + 1) % 3; // Cycle: 0 -> 1 -> 2
            keyDownPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            keyDownPressed = true;
        }

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && enterPressed) {
            if (currentSelection == 0) { // Start
                inMenu = false;
                audio.stopMusic();
            }
            else if (currentSelection == 1) { // Controls
                ControlsMenu controlsMenu(window, textRenderer, controller, shader);
                controlsMenu.display();
            }
            else if (currentSelection == 2) { // Exit
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                audio.stopMusic();
            }
            enterPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            enterPressed = true;
        }
    }

    void handleControllerInput(bool& inMenu) {
        static bool dpadUpPressed = false;
        static bool dpadDownPressed = false;
        static bool aButtonPressed = false;

        if (!controller.isConnected()) return;

        if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP) && dpadUpPressed) {
            currentSelection = (currentSelection - 1 + 3) % 3; // 2 -> 1 -> 0
            dpadUpPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP)) {
            dpadUpPressed = true;
        }

        if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN) && dpadDownPressed) {
            currentSelection = (currentSelection + 1) % 3; // 0 -> 1 -> 2
            dpadDownPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN)) {
            dpadDownPressed = true;
        }

        if (!controller.isButtonPressed(XINPUT_GAMEPAD_A) && aButtonPressed) {
            if (currentSelection == 0) { // Start
                inMenu = false;
                audio.stopMusic();
            }
            else if (currentSelection == 1) { // Controls
                ControlsMenu controlsMenu(window, textRenderer, controller, shader);
                controlsMenu.display();
            }
            else if (currentSelection == 2) { // Exit
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                audio.stopMusic();
            }
            aButtonPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
            aButtonPressed = true;
        }
    }


};

