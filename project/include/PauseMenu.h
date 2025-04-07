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

/**
pause menu

todo: look at mainmmenu.h todo

ngl this was basically copy and pasted from the main menu
 */
class PauseScreen {
public:
    PauseScreen(Windowing& window, TTF& textRenderer, Controller& controller, AudioSystem& audio)
        : window(window), textRenderer(textRenderer), controller(controller), audio(audio),
        resumeButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        quitButton(0, 0, 0, 0, glm::vec3(0, 0, 0)), currentSelection(0)
    {
        shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
        setupButtons();
        setupText();
        loadBackgroundTexture();
    }

    ~PauseScreen() {
        glDeleteTextures(1, &backgroundTexture);
        uiText.clear();
        delete shader;
    }

    //bool to check if paused
    bool displayPauseScreen() {
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        audio.pauseMusic();
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

            // Handle Resume and Quit buttons on release
            bool isEnterPressed = controller.isButtonPressed(XINPUT_GAMEPAD_A) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS;

            if (wasButtonPressed && !isEnterPressed) {
                // The Enter button was just released, perform the action
                if (currentSelection == 0) {
                    audio.resumePauseSounds();
                    return true;  // Resume game
                }
                else if (currentSelection == 1) {
                    return false;  // Quit game
                }
            }

            // Update the previous Enter button state
            wasButtonPressed = isEnterPressed;

            // Handle Down arrow/DPAD Down for selection change
            bool isDownPressed = controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS;

            if (wasDownPressed && !isDownPressed) {
                // The Down button was just released, change selection
                currentSelection = (currentSelection + 1) % 2;  // Toggle between resume and quit
            }

            // Update the previous Down button state
            wasDownPressed = isDownPressed;

            // Handle Up arrow/DPAD Up for selection change
            bool isUpPressed = controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS;

            if (wasUpPressed && !isUpPressed) {
                // The Up button was just released, change selection
                currentSelection = (currentSelection + 1) % 2;  // Toggle between resume and quit
            }

            // Update the previous Up button state
            wasUpPressed = isUpPressed;
        }

        return false;
    }


private:
    Windowing& window;
    TTF& textRenderer;
    Controller& controller;
    AudioSystem& audio;
    unsigned int backgroundTexture;
    Shader* shader;
    Button resumeButton, quitButton;
    std::vector<Text> uiText;
    int currentSelection;

    void setupButtons() {
        int w, h;
        glfwGetWindowSize(window.getGLFWwindow(), &w, &h);
        float width = 0.25f * w;
        float height = 0.07f * h;
        float x = 0.375f * w;

        resumeButton = Button(x, 0.4f * h, width, height, glm::vec3(1, 0, 0));
        quitButton = Button(x, 0.52f * h, width, height, glm::vec3(0, 0, 0));

    }

    void setupText() {
        int w, h;
        glfwGetWindowSize(window.getGLFWwindow(), &w, &h);
        float scale = std::min(w, h) * 0.001f;

        uiText.emplace_back("Resume", w * 0.5f, h * 0.43f, scale, glm::vec3(1, 1, 1));
        uiText.emplace_back("Quit", w * 0.5f, h * 0.55f, scale, glm::vec3(1, 1, 1));
        uiText.emplace_back("Paused", w * 0.5f, h * 0.2f, scale * 1.5f, glm::vec3(1, 1, 0));
    }

    void renderOverlay() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();


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

    void drawBackground() {
        float vertices[] = {
            // positions     // texture coords
            -1.0f,  1.0f,    0.0f, 1.0f,  // top-left
            -1.0f, -1.0f,    0.0f, 0.0f,  // bottom-left
             1.0f, -1.0f,    1.0f, 0.0f,  // bottom-right
            -1.0f,  1.0f,    0.0f, 1.0f,  // top-left
             1.0f, -1.0f,    1.0f, 0.0f,  // bottom-right
             1.0f,  1.0f,    1.0f, 1.0f   // top-right
        };

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        shader->use();
        shader->setInt("useTexture", 1);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void loadBackgroundTexture() {
        glGenTextures(1, &backgroundTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("project/assets/background/backgrounMainMenu.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            printf("Failed to load background texture\n");
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_set_flip_vertically_on_load(false);
    }

    void renderText() {
        for (const auto& text : uiText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }

};
