#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include "Controller.h"
#include <BackgroundRenderer.h>
#include <vector>
#include <string>
#include <Text.h>
#include "TransparentBoxRenderer.h"
#include <AudioSystem.h>
#include <iomanip>

class EndScreen {
public:
    EndScreen(Windowing& window, TTF& textRenderer, Controller& controller, AudioSystem& audio, GameState& gameState);
    ~EndScreen();
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
    GameState& gameState;

    Shader* shader;
    BackgroundRenderer* backgroundRenderer;

    std::vector<Text> uiText;
    float textScale = 1.0f;

    int windowWidth, windowHeight;
    int currentSelection = 0;

    Button startButton;
    TransparentBoxRenderer boxRenderer;
    AudioSystem& audio;
    bool audioInitialized = false;
};
