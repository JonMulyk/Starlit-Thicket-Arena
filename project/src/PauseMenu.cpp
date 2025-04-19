#include "PauseMenu.h"


PauseScreen::PauseScreen(Windowing& window, TTF& textRenderer, Controller& controller, AudioSystem& audio)
    : window(window), textRenderer(textRenderer), controller(controller), audio(audio),
    resumeButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    quitButton(0, 0, 0, 0, glm::vec3(0, 0, 0)), currentSelection(0)
{
    shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
    setupButtons();
    setupText();
    //loadBackgroundTexture();
}

PauseScreen::~PauseScreen() {
    glDeleteTextures(1, &backgroundTexture);
    uiText.clear();
    delete shader;
}

bool PauseScreen::displayPauseScreen() {
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    audio.pauseMusic();
    backgroundRenderer = new BackgroundRenderer("project/assets/background/pauseScreen.jpg", shader);

    bool running = true;
    bool wasButtonPressed = false;
    bool wasUpPressed = false;
    bool wasDownPressed = false;
    while (!window.shouldClose() && running) {
        window.clear();

        renderOverlay();
        renderText();

        window.swapBuffer();
        glfwPollEvents();
        controller.Update();

        bool isEnterPressed = controller.isButtonPressed(XINPUT_GAMEPAD_A) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS;

        if (wasButtonPressed && !isEnterPressed) {
            if (currentSelection == 0) {
                audio.resumePauseSounds();
                return true;
            }
            else if (currentSelection == 1) {
                return false;
            }
        }

        wasButtonPressed = isEnterPressed;

        bool isDownPressed = controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS;

        if (wasDownPressed && !isDownPressed) {
            currentSelection = (currentSelection + 1) % 2;
        }

        wasDownPressed = isDownPressed;

        bool isUpPressed = controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS;

        if (wasUpPressed && !isUpPressed) {
            currentSelection = (currentSelection + 1) % 2;
        }

        wasUpPressed = isUpPressed;
    }

    return false;
}

void PauseScreen::setupButtons() {
    int w, h;
    glfwGetWindowSize(window.getGLFWwindow(), &w, &h);
    float width = 0.25f * w;
    float height = 0.07f * h;
    float x = 0.375f * w;

    resumeButton = Button(x, 0.4f * h, width, height, glm::vec3(1, 0, 0));
    quitButton = Button(x, 0.52f * h, width, height, glm::vec3(0, 0, 0));
}

void PauseScreen::setupText() {
    int w, h;
    glfwGetWindowSize(window.getGLFWwindow(), &w, &h);
    float scale = std::min(w, h) * 0.001f;

    uiText.emplace_back("Resume", w * 0.5f, h * 0.43f, scale, glm::vec3(1, 1, 1));
    uiText.emplace_back("Main Menu", w * 0.5f, h * 0.55f, scale, glm::vec3(1, 1, 1));
    uiText.emplace_back("Paused", w * 0.5f, h * 0.2f, scale * 1.5f, glm::vec3(1, 1, 0));
}

void PauseScreen::renderOverlay() {
    glDisable(GL_DEPTH_TEST);
    backgroundRenderer->draw();

    resumeButton.draw(shader, window.getWidth(), window.getHeight());
    quitButton.draw(shader, window.getWidth(), window.getHeight());

    if (currentSelection == 0) {
        resumeButton.setColor(glm::vec3(1, 0, 0));
        quitButton.setColor(glm::vec3(0, 1, 0));
    }
    else if (currentSelection == 1) {
        resumeButton.setColor(glm::vec3(0, 1, 0));
        quitButton.setColor(glm::vec3(1, 0, 0));
    }
}

void PauseScreen::renderText() {
    for (const auto& text : uiText) {
        textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
    }
}
