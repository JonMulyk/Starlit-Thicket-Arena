#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include "Controller.h"
#include <ControlMenu.h>
#include <BackgroundRenderer.h>
#include <vector>
#include <string>
#include <Text.h>
#include <AudioSystem.h>

class MainMenu {
public:
    MainMenu(Windowing& window, TTF& textRenderer, Controller& controller);
    ~MainMenu();
    void displayMenu();

private:
    void renderMenu();
    void initializeUIText();
    void renderText(const std::vector<Text>& renderingText);
    void compileShaders();
    void handleKeyboardInput(bool& inMenu);
    void handleControllerInput(bool& inMenu);

    Controller& controller;
    Windowing& window;
    TTF& textRenderer;
    Shader* shader;
    BackgroundRenderer* backgroundRenderer;
    std::vector<Text> uiText;
    int windowWidth, windowHeight;

    Button startButton, controlsButton, exitButton;
    int currentSelection = 0;
    AudioSystem audio;
    bool audioInitialized = false;

    const std::string startText = "Start";
    const std::string controlsText = "Controls";
    const std::string exitText = "Exit";
};
