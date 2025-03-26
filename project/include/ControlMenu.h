#ifndef CONTROLS_H
#define CONTROLS_H

#include "Windowing.h"
#include "TTF.h"
#include "Controller.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <vector>

/*
TODO:

*/
class ControlsMenu {
public:
    ControlsMenu(Windowing& window, TTF& textRenderer, Controller& controller, Shader* shader)
        : window(window), textRenderer(textRenderer), controller(controller), shader(shader) {
        loadControlsBackground();
        initializeControlsText();
    }

    //controls menu stuff
    void display() {
        bool inControls = true;
        while (inControls && !window.shouldClose()) {

            window.clear();
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            drawBackground();
            renderText(controlsText);
            drawTransparentBox(0.3f, 0.3f, 0.8f, 0.8f, 0.5f); 
            window.swapBuffer();
            glfwPollEvents();
            handleKeyboardInput(inControls);
            handleControllerInput(inControls);
        }
    }

private:
    Windowing& window;
    TTF& textRenderer;
    Controller& controller; 
    Shader* shader;
    unsigned int backgroundTexture;
    std::vector<Text> controlsText;
    int windowWidth, windowHeight;


    //intialize stuff here
    void initializeControlsText() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
        float buttonX = 0.5f * windowWidth;
        float buttonY = 0.5f * windowHeight;
        Text Start = Text("Start", buttonX + 60, buttonY + 160.0f, 1.0f, glm::vec3(1, 1, 1));
        Text Exit = Text("Exit", buttonX + 60.0f, buttonY - 60, 1.0f, glm::vec3(1, 1, 1));
        controlsText.push_back(Start);
        controlsText.push_back(Exit);
    }

    // Render the text from the vector
    void renderText(const std::vector<Text>& renderingText) {
        for (const auto& text : renderingText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }

    //same as code from main menu
    void loadControlsBackground() {
        glGenTextures(1, &backgroundTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("project/assets/background/controlsBackground.jpg", &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            printf("Failed to load controls background texture\n");
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_set_flip_vertically_on_load(false);
    }

    void drawTransparentBox(float x, float y, float width, float height, float alpha) {
        Shader boxShader("project/assets/shaders/transparentBoxShader.vert", "project/assets/shaders/transparentBoxShader.frag"); 

        float vertices[] = {
            x,         y + height, 0.0f, 1.0f,
            x,         y,          0.0f, 0.0f,
            x + width, y,          1.0f, 0.0f,
            x,         y + height, 0.0f, 1.0f,
            x + width, y,          1.0f, 0.0f,
            x + width, y + height, 1.0f, 1.0f
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

        boxShader.use();
        boxShader.setVec4("boxColor", glm::vec4(0.0f, 0.0f, 0.0f, alpha));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }


    // Draw the background
    void drawBackground() {
        float vertices[] = {
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


    void handleKeyboardInput(bool& inControls) {
        static bool enterPressed = false;
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && enterPressed) {
            inControls = false;
            enterPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            enterPressed = true;
        }
    }


    void handleControllerInput(bool& inControls) {
        static bool aButtonPressed = false;
        if (!controller.isButtonPressed(XINPUT_GAMEPAD_A) && aButtonPressed) {
            inControls = false;
            aButtonPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
            aButtonPressed = true;
        }
    }
};

#endif // CONTROLS_H