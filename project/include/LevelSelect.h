#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include "Button.h"
#include "Windowing.h"
#include "Text.h"
#include "Controller.h"
#include <vector>
#include <BackgroundRenderer.h>
#include <AudioSystem.h>

class LevelSelectMenu {
public:
    LevelSelectMenu(Windowing& window, TTF& textRenderer, Controller& controller, GameState& gameState);
    ~LevelSelectMenu();

    int displayMenuLevel();

private:
    void renderMenu();
    void initializeUIText();
    void compileShaders();
    void renderText(const std::vector<Text>& renderingText);
    void handleKeyboardInput(int& selectedLevel);
    void handleControllerInput(int& selectedLevel);

    Controller& controller;
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    BackgroundRenderer* backgroundRenderer;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button level1Button, level2Button, level3Button, backButton, trailsButton;
    AudioSystem audio;
    bool audioInitialized;
    GameState& gameState;
    int currentSelection = 0; 
};
