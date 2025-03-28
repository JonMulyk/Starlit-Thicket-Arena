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
        : window(window), textRenderer(textRenderer), controller(controller), shader(shader), backButton(0, 0, 0, 0, glm::vec3(0, 0, 0)) 
    {
        loadControlsBackground();
        initializeControlsText();
        boxShader = new Shader("project/assets/shaders/transparentBoxShader.vert", "project/assets/shaders/transparentBoxShader.frag");
    }

    ~ControlsMenu() {
        glDeleteTextures(1, &backgroundTexture);
        controlsText.clear();
        delete boxShader; 
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
            drawTransparentBox(-0.6f, -0.6f, 1.2f, 1.2f, 0.5f);
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


    //intialize stuff here
    void initializeControlsText() {

        float screenWidth = static_cast<float>(windowWidth);
        float screenHeight = static_cast<float>(windowHeight);


        float buttonX = 0.5f * screenWidth;
        float buttonY = 0.5f * screenHeight;
        controlsText.push_back(Text("Keyboard", buttonX + 300, buttonY + 1100.0f, 1.0f, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Controller", buttonX + 800.0f, buttonY + 1100, 1.0f, glm::vec3(1, 1, 1)));
        
        //keyboard
        controlsText.push_back(Text("w - Accelerate", buttonX + 300, buttonY + 1050.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("a - Left", buttonX + 300, buttonY + 1000.0f, 0.6, glm::vec3(1, 1, 1)));
        //controlsText.push_back(Text("s - Break", buttonX + 300, buttonY + 950.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("d - Right", buttonX + 300, buttonY + 950.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("space - Boost", buttonX + 300, buttonY + 900.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("mouse - Camera", buttonX + 300, buttonY + 850.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("scroll wheel - zoom", buttonX + 300, buttonY + 800.0f, 0.6, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("escape - Exit", buttonX + 300, buttonY + 750.0f, 0.6, glm::vec3(1, 1, 1)));

        //controller
        controlsText.push_back(Text("Left Joystick - Turn", buttonX + 800.0f, buttonY + 1050, 0.6f, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Right Joystick - Camera", buttonX + 800.0f, buttonY + 1000, 0.6f, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("Right Trigger - Accelerate", buttonX + 800.0f, buttonY + 950, 0.6f, glm::vec3(1, 1, 1)));
        //controlsText.push_back(Text("Left Trigger - Break", buttonX + 800.0f, buttonY + 900, 0.6f, glm::vec3(1, 1, 1)));
        controlsText.push_back(Text("B - Boost", buttonX + 800.0f, buttonY + 900, 0.6f, glm::vec3(1, 1, 1)));


        controlsText.push_back(Text("Press B (controller) or Backspace to return", buttonX + 550.0f, buttonY + 400, 0.6f, glm::vec3(1, 1, 1)));
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

        boxShader->use();
        boxShader->setVec4("boxColor", glm::vec4(0.0f, 0.0f, 0.0f, alpha));

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
        if (!controller.isButtonPressed(XINPUT_GAMEPAD_B) && aButtonPressed) {
            inControls = false;
            aButtonPressed = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_B)) {
            aButtonPressed = true;
        }
    }
};

#endif // CONTROLS_H