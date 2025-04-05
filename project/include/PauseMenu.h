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
        resumeButton(0, 0, 0, 0, glm::vec3(1, 1, 1)),
        quitButton(0, 0, 0, 0, glm::vec3(1, 1, 1)), currentSelection(0)
    {
        shader = new Shader("PauseShader", "project/assets/shaders/overlayShader.vert", "project/assets/shaders/overlayShader.frag");
        setupButtons();
        setupText();
    }

    ~PauseScreen() {
        delete shader;
    }

    //bool to check if paused
    bool displayPauseScreen() {
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        audio.pauseMusic();
        bool running = true;

        while (!window.shouldClose() && running) {
            window.clear();

            renderOverlay();
            renderText();

            window.swapBuffer();
            glfwPollEvents();
            controller.Update();

            if (controller.isButtonJustReleased(XINPUT_GAMEPAD_A) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
                if (currentSelection == 0) {
                    audio.resumePauseSounds();
                    return true;
                }
                else if (currentSelection == 1) {
                    glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                    return false;
                }
            }

\
            if (controller.isButtonJustReleased(XINPUT_GAMEPAD_DPAD_DOWN) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
                currentSelection = (currentSelection + 1) % 2;
            }
            if (controller.isButtonJustReleased(XINPUT_GAMEPAD_DPAD_UP) || glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS) {
                currentSelection = (currentSelection + 1) % 2;
            }
        }

        return false;
    }

private:
    Windowing& window;
    TTF& textRenderer;
    Controller& controller;
    AudioSystem& audio;

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

        resumeButton = Button(x, 0.4f * h, width, height, glm::vec3(1, 1, 1));
        quitButton = Button(x, 0.52f * h, width, height, glm::vec3(1, 1, 1));
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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        //change this for picutre later i guess idk
        float vertices[] = {
            -1.0f, -1.0f, 0.6f,
             1.0f, -1.0f, 0.6f,
             1.0f,  1.0f, 0.6f,
            -1.0f,  1.0f, 0.6f,
        };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        shader->use();
        shader->setVec4("overlayColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        //does not work for some reason wtf
        resumeButton.setColor(currentSelection == 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 1, 1));
        quitButton.setColor(currentSelection == 1 ? glm::vec3(0.8f, 0.2f, 0.2f) : glm::vec3(1, 1, 1));

        resumeButton.draw(shader, window.getWidth(), window.getHeight());
        quitButton.draw(shader, window.getWidth(), window.getHeight());

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void renderText() {
        for (const auto& text : uiText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }
};
