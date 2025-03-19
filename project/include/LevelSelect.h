#pragma once
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include <Windowing.h>
#include <vector>
#include <Text.h>

class LevelSelectMenu {
public:
    LevelSelectMenu(Windowing& window, TTF& textRenderer)
        : window(window), textRenderer(textRenderer),
        level1Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        level2Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        level3Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        backButton(0, 0, 0, 0, glm::vec3(0, 0, 0))
    {
        compileShaders();
        loadBackgroundTexture();
    }

    ~LevelSelectMenu() {
        glDeleteTextures(1, &backgroundTexture);
    }

    int displayMenuLevel() {
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        int selectedLevel = 0; // 0 means no selection yet
        while (selectedLevel == 0 && !window.shouldClose()) {
            window.clear();
            renderMenu();
            window.swapBuffer();
            glfwPollEvents();
            handleKeyboardInput(selectedLevel);


            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
                if (level1Button.isClicked(xpos, ypos)) selectedLevel = 1;
                else if (level2Button.isClicked(xpos, ypos)) selectedLevel = 2;
                else if (level3Button.isClicked(xpos, ypos)) selectedLevel = 3;
                else if (backButton.isClicked(xpos, ypos)) selectedLevel = -1;
            }
        }
        return selectedLevel;
    }

private:
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button level1Button, level2Button, level3Button, backButton;

    int currentSelection = 0;

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

        // Clear uiText
        uiText.clear();

        // Add title
        float centerX = 0.5f * windowWidth;
        float titleY = 0.2f * windowHeight;
        uiText.push_back(Text("Select Level", centerX, titleY, 1.0f, glm::vec3(1, 1, 1)));

        // Define button layout
        float buttonY = 0.85f * windowHeight;  // Moved even lower
        float buttonHeight = 0.0625f * windowHeight;
        float buttonWidth = 0.2f * windowWidth;
        float spacing = 0.05f * windowWidth;
        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float startX = (windowWidth - totalWidth) / 2;

        // Position buttons
        level1Button = Button(startX, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
        level2Button = Button(startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
        level3Button = Button(startX + 2 * (buttonWidth + spacing), buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
        backButton = Button(startX + 3 * (buttonWidth + spacing), buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));

        // Add button texts
        float textY = buttonY + buttonHeight / 2;  // Center text on buttons
        uiText.push_back(Text("Level 1", startX + buttonWidth / 2, textY, 1.0f, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Level 2", startX + buttonWidth + spacing + buttonWidth / 2, textY, 1.0f, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Level 3", startX + 2 * (buttonWidth + spacing) + buttonWidth / 2, textY, 1.0f, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Back", startX + 3 * (buttonWidth + spacing) + buttonWidth / 2, textY, 1.0f, glm::vec3(1, 1, 1)));


        level1Button.setColor(currentSelection == 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));  // Green for selected
        level2Button.setColor(currentSelection == 1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        level3Button.setColor(currentSelection == 2 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        backButton.setColor(currentSelection == 3 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));

        // Draw buttons
        level1Button.draw(shader, windowWidth, windowHeight);
        level2Button.draw(shader, windowWidth, windowHeight);
        level3Button.draw(shader, windowWidth, windowHeight);
        backButton.draw(shader, windowWidth, windowHeight);


        // Render text
        renderText(uiText);
    }

    void compileShaders() {
        shader = new Shader("LevelSelectShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
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

    void drawBackground() {
        float vertices[] = {
            -1.0f,  1.0f,    0.0f, 1.0f,
            -1.0f, -1.0f,    0.0f, 0.0f,
             1.0f, -1.0f,    1.0f, 0.0f,
            -1.0f,  1.0f,    0.0f, 1.0f,
             1.0f, -1.0f,    1.0f, 0.0f,
             1.0f,  1.0f,    1.0f, 1.0f
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

    void renderText(const std::vector<Text>& renderingText) {
        for (const auto& text : renderingText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }

    void handleKeyboardInput(int& selectedLevel) {
        static bool keyUpPressed = false;
        static bool keyDownPressed = false;
        static bool keyEnterPressed = false;

        // Handle Up Key
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            if (!keyUpPressed) {
                currentSelection = (currentSelection - 1 + 4) % 4;
                keyUpPressed = true;
                keyEnterPressed = false; // Reset Enter press status when navigation occurs
            }
        }
        else {
            keyUpPressed = false;
        }

        // Handle Down Key
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (!keyDownPressed) {
                currentSelection = (currentSelection + 1) % 4;
                keyDownPressed = true;
                keyEnterPressed = false; // Reset Enter press status when navigation occurs
            }
        }
        else {
            keyDownPressed = false;
        }

        // Handle Enter Key
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_T) == GLFW_PRESS) {
            if (!keyEnterPressed) {
                if (currentSelection == 3) {
                    selectedLevel = -1;  // Back button pressed
                }
                else if (currentSelection != 3) {
                    selectedLevel = currentSelection + 1;  // Start game with selected level
                }
                keyEnterPressed = true;
            }
        }
        else {
            keyEnterPressed = false;
        }
    }
    
};