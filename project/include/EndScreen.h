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
#include <AudioSystem.h>

class EndScreen {
public:
    EndScreen(Windowing& window, TTF& textRenderer, Controller& controller);
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

    Shader* shader;
    BackgroundRenderer* backgroundRenderer;

    std::vector<Text> uiText;

    int windowWidth, windowHeight;
    int currentSelection = 0;

    Button startButton;

    AudioSystem audio;
    bool audioInitialized = false;
};
