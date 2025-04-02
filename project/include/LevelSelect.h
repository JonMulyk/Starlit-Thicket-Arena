#pragma once
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include <Windowing.h>
#include <vector>
#include <Text.h>
#include <Controller.h>


/*
TODO: organize to seperate from cpp and .h file, essentially optimize and clean up after testing

currently frontloaded in .h for functionality sakes (none of this code is really reusable)

bug: unless we reinitialize the audio, when the game resets, the music does not play
*/
class LevelSelectMenu {
public:
    LevelSelectMenu(Windowing& window, TTF& textRenderer, Controller& controller, GameState& gameState)
        : window(window), textRenderer(textRenderer), controller(controller), gameState(gameState),
        level1Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        level2Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        level3Button(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        backButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
		trailsButton(0, 0, 0, 0, glm::vec3(0, 0, 0))
    {
        initializeUIText();
        compileShaders();
        loadBackgroundTexture();
    }

    /*
    cleanup
    */
    ~LevelSelectMenu() {
        glDeleteTextures(1, &backgroundTexture);
        uiText.clear();
    }

    /*
    will return
    */
    int displayMenuLevel() {
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (!audioInitialized) {
            audio.init();
            audio.startLevelMusic();
            audioInitialized = true;
        }
        audio.startLevelMusic();
        int selectedLevel = 0; // 0 means no selection yet
        static bool trailsButtonClicked = false; // debounce flag for trails button
        while (selectedLevel == 0 && !window.shouldClose()) {
            window.clear();
            renderMenu();
            window.swapBuffer();
            glfwPollEvents();
            handleKeyboardInput(selectedLevel);
            handleControllerInput(selectedLevel);

            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);

                if (level1Button.isClicked(xpos, ypos)) {
                    selectedLevel = 1;
                    audio.stopMusic();
                }
                else if (level2Button.isClicked(xpos, ypos)) {
                    selectedLevel = 2;
                    audio.stopMusic();
                }
                else if (level3Button.isClicked(xpos, ypos)) {
                    selectedLevel = 3;
                    audio.stopMusic();
                }
                else if (backButton.isClicked(xpos, ypos)) {
                    selectedLevel = -1;
                    audio.stopMusic();
                }
                else if (trailsButton.isClicked(xpos, ypos)) {
                    if (!trailsButtonClicked) {  // only toggle on first press
                        gameState.tempTrails = !gameState.tempTrails;
                        std::cout << "changing trails: " << gameState.tempTrails << std::endl;
                        trailsButtonClicked = true;
                    }
                }
            }
            else {
                trailsButtonClicked = false; // reset debounce when mouse button is released
            }
        }
        return selectedLevel; // 1 = level 1, 2 = level 2, 3 = level 3, -1 = back
    }


private:

    Controller& controller;
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button level1Button, level2Button, level3Button, backButton, trailsButton;
    AudioSystem audio;
    bool audioInitialized;
    GameState& gameState;
    int currentSelection = 0;

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

        //horizontal button conversion, can be modifeid to be different
        float buttonY = 0.85f * windowHeight;
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

        trailsButton = Button(0.4f * windowWidth, 0.65f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));


        //green if currentSelected, otherwise red, somewhat different from mainMenu
        level1Button.setColor(currentSelection == 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        level2Button.setColor(currentSelection == 1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        level3Button.setColor(currentSelection == 2 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        backButton.setColor(currentSelection == 3 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
        trailsButton.setColor(gameState.tempTrails ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));

        level1Button.draw(shader, windowWidth, windowHeight);
        level2Button.draw(shader, windowWidth, windowHeight);
        level3Button.draw(shader, windowWidth, windowHeight);
        backButton.draw(shader, windowWidth, windowHeight);
		trailsButton.draw(shader, windowWidth, windowHeight);

        //render text
        renderText(uiText);
    }


    void initializeUIText() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

        //float buttonX = 0.5f * windowWidth;
        //float buttonY = 0.5f * windowHeight;

        //horizontal button conversion, can be modifeid to be different
        //float buttonY = 0.85f * windowHeight;
        float buttonHeight = 0.1 * windowHeight;
        float buttonWidth = 0.2f * windowWidth;
        float spacing = 0.05f * windowWidth;
        float totalWidth = 4 * buttonWidth + 3 * spacing;
        float shiftRight = 0.1f * windowWidth;
        float startX = (windowWidth - totalWidth) / 2 + shiftRight; //cursed 2.0
        float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;


        uiText.push_back(Text("Stage 1", startX, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Stage 2", startX + buttonWidth + spacing, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Stage 3", startX + 2 * (buttonWidth + spacing), buttonHeight, baseScale, glm::vec3(1, 1, 1)));
        uiText.push_back(Text("Back", startX + 3 * (buttonWidth + spacing), buttonHeight, baseScale, glm::vec3(1, 1, 1)));
		uiText.push_back(Text("Y - Temp Trails", (0.5f) * windowWidth, 0.3f * windowHeight, baseScale, glm::vec3(1, 1, 1)));

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
        unsigned char* data = stbi_load("project/assets/background/levelSelect.jpg", &width, &height, &nrChannels, 0);
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
        static bool keyUpReleased = false;  
        static bool keyDownReleased = false;  
        static bool keyEnterReleased = false;  

 
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_RELEASE && keyUpReleased) {
            currentSelection = (currentSelection - 1 + 4) % 4;
            keyUpReleased = false; 
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
            keyUpReleased = true;  
        }


        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_RELEASE && keyDownReleased) {
            currentSelection = (currentSelection + 1) % 4; 
            keyDownReleased = false;  
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
            keyDownReleased = true;   
        }


        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && keyEnterReleased) {
            if (currentSelection == 3) {
                selectedLevel = -1; 
                audio.stopMusic();
            }
            else if (currentSelection != 3) {
                selectedLevel = currentSelection + 1;
                audio.stopMusic();
            }
            keyEnterReleased = false; 
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            keyEnterReleased = true;   
        }
    }

    void handleControllerInput(int& selectedLevel) {
        static bool dpadUpReleased = false; 
        static bool dpadDownReleased = false; 
        static bool aButtonReleased = false;  
        static bool trailsButtonClicked = false;

        if (!controller.isConnected()) return;

        if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_LEFT) && dpadUpReleased) {
            currentSelection = (currentSelection - 1 + 4) % 4;
            dpadUpReleased = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT)) {
            dpadUpReleased = true;  
        }

        if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_RIGHT) && dpadDownReleased) {
            currentSelection = (currentSelection + 1) % 4;
            dpadDownReleased = false; 
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) {
            dpadDownReleased = true;  
        }

        if (controller.isButtonReleased(XINPUT_GAMEPAD_A) && aButtonReleased) {
            if (currentSelection == 3) {
                selectedLevel = -1; 
                audio.stopMusic();
            }
            else if (currentSelection != 3) {
                selectedLevel = currentSelection + 1; 
                audio.stopMusic();
            }
            aButtonReleased = false;
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
            aButtonReleased = true; 
        }


        if (controller.isButtonPressed(XINPUT_GAMEPAD_Y)) {
            if (!trailsButtonClicked) {  // only toggle on first press
                gameState.tempTrails = !gameState.tempTrails;
                trailsButtonClicked = true;
            }
        }
        else {
            trailsButtonClicked = false;
        }


    }
};