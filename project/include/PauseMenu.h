#pragma once

#include "Windowing.h"
#include "Shader.h"
#include "Button.h"
#include "Controller.h"
#include "TTF.h"
#include "AudioSystem.h"
#include <vector>
#include <glm.hpp>
#include <Text.h>
#include <BackgroundRenderer.h>
#include <stb_image.h>

class PauseScreen {
public:
    PauseScreen(Windowing& window, TTF& textRenderer, Controller& controller, AudioSystem& audio);
    ~PauseScreen();

    bool displayPauseScreen();

private:
    Windowing& window;
    TTF& textRenderer;
    Controller& controller;
    AudioSystem& audio;
    unsigned int backgroundTexture;
    Shader* shader;
    BackgroundRenderer* backgroundRenderer;
    Button resumeButton, quitButton;
    std::vector<Text> uiText;
    int currentSelection;

    void setupButtons();
    void setupText();
    void renderOverlay();
    void drawBackground();
    void loadBackgroundTexture();
    void renderText();
};
