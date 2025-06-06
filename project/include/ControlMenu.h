#pragma once

#include "Windowing.h"
#include "TTF.h"
#include "Controller.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "TransparentBoxRenderer.h"
#include <Text.h>


/*
TODO:

put in seperate .h and .cpp file
*/
class ControlsMenu {
public:
    ControlsMenu(Windowing& window, TTF& textRenderer, Controller& controller, Shader* shader)
        : window(window), textRenderer(textRenderer), controller(controller), shader(shader), backButton(0, 0, 0, 0, glm::vec3(0, 0, 0)) 
    {
        loadControlsBackground();
        initializeControlsText();
        //boxShader = new Shader("project/assets/shaders/transparentBoxShader.vert", "project/assets/shaders/transparentBoxShader.frag");
    }

    ~ControlsMenu() {
        glDeleteTextures(1, &backgroundTexture);
        controlsText.clear();
        //delete boxShader; 
        //delete shader;

    }

    //controls menu stuff
    void display() {
        bool inControls = true;
        while (inControls && !window.shouldClose()) {

            window.clear();
            //glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            drawBackground();
            //drawTransparentBox(-0.6f, -0.6f, 1.2f, 1.2f, 0.5f);
            boxRenderer.draw(-0.6f, -0.6f, 1.2f, 1.2f, 0.5f);
            renderText(controlsText);
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
    Shader* boxShader;
    Button backButton;
    TransparentBoxRenderer boxRenderer;


    //intialize stuff here
    void initializeControlsText() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        float keyboardX = 0.35f * windowWidth; //anchors
        //float keyboardY = 0.5f * windowHeight;
        float baseScale = std::min(windowWidth, windowHeight) * 0.00055f;

        float controllerX = 0.65f * windowWidth;

        controlsText.push_back(Text("Keyboard", keyboardX, windowHeight * 0.75, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Controller", controllerX, windowHeight * 0.75, baseScale, glm::vec3(1, 1, 1)));
        
        //keyboard
        controlsText.push_back(Text("W - Accelerate", keyboardX, windowHeight * 0.70, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("A - Left", keyboardX, windowHeight * 0.65, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("R - Right", keyboardX, windowHeight * 0.6, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("space - Boost", keyboardX, windowHeight * 0.55, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("mouse - Camera", keyboardX, windowHeight * 0.50, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("p - pause", keyboardX, windowHeight * 0.45, baseScale, glm::vec3(1, 1, 1)));
        //controlsText.push_back(Text("escape - Exit", keyboardX, windowHeight * 0.40, baseScale, glm::vec3(1, 1, 1))); //we don't need this

        //controller
        controlsText.push_back(Text("Left Joystick - Turn", controllerX, windowHeight * 0.70, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Right Joystick - Camera", controllerX, windowHeight * 0.65, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Right Trigger - Accelerate", controllerX, windowHeight * 0.60, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("B - Boost", controllerX, windowHeight * 0.55, baseScale, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Menu Button - Pause", controllerX, windowHeight * 0.50, baseScale, glm::vec3(1, 1, 1)));


        controlsText.push_back(Text("Press B or Backspace to return", 0.5f * windowWidth, windowHeight * 0.25, baseScale, glm::vec3(1, 1, 1)));
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
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_BACKSPACE) == GLFW_RELEASE && enterPressed) {
            inControls = false;
            enterPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
            enterPressed = true;
        }
    }


    void handleControllerInput(bool& inControls) {
        static bool aButtonPressed = false;

        if (!controller.isConnected()) return;
        if (!controller.isButtonPressed(XINPUT_GAMEPAD_B) && aButtonPressed) {
            inControls = false;
            aButtonPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_B)) {
            aButtonPressed = true;
        }
    }
};

