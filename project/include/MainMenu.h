#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>
#include "Controller.h" 

/*
TODO: organize to seperate from cpp and .h file, essentially optimize and clean up after testing

currently frontloaded in .h for functionality sakes (none of this code is really reusable)

bug: unless we reinitialize the audio, when the game resets, the music does not play
*/
class MainMenu {
public:
    MainMenu(Windowing& window, TTF& textRenderer, Controller& controller)
        : window(window), textRenderer(textRenderer), controller(controller),
        startButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
        exitButton(0, 0, 0, 0, glm::vec3(0, 0, 0))
    {
        compileShaders();
        loadBackgroundTexture();
        initializeUIText();
        initializeButtons(); 
    }

    ~MainMenu() {
        glDeleteTextures(1, &backgroundTexture);
        uiText.clear();
    }

    void displayMenu() {
        if (!audioInitialized) {
            audio.init();
            audio.startMenuMusic();
            audioInitialized = true;
        }
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        bool inMenu = true;

        audio.startMenuMusic();  
        while (inMenu && !window.shouldClose()) {
            window.clear();
            renderMenu();
            window.swapBuffer();
            glfwPollEvents();
            handleControllerInput(inMenu);
            handleKeyboardInput(inMenu);

            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
                glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

                if (startButton.isClicked(xpos, ypos)) {
                    inMenu = false;
                    //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    audio.stopMusic();
                }
                if (exitButton.isClicked(xpos, ypos)) {
                    glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                }
            }
        }
    }

private:
    Controller& controller;
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button startButton, exitButton;
    int currentSelection;   
    AudioSystem audio;
    bool audioInitialized;

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();

        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        startButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
        exitButton = Button(0.4f * windowWidth, 0.65f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));

        if (currentSelection == 0) {
            startButton.setColor(glm::vec3(0, 1, 0));  // Green for selected
            exitButton.setColor(glm::vec3(1, 0, 0));   // Red for unselected
        }
        else {
            startButton.setColor(glm::vec3(1, 0, 0));  // Red for unselected
            exitButton.setColor(glm::vec3(0, 1, 0));   // Green for selected
        }

        startButton.draw(shader, windowWidth, windowHeight);
        exitButton.draw(shader, windowWidth, windowHeight);

        renderText(uiText);
    }


    void initializeUIText() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

        float buttonX = 0.5f * windowWidth;
        float buttonY = 0.5f * windowHeight;

        Text Start = Text("Start", buttonX + 60, buttonY + 160.0f, 1.0f, glm::vec3(1, 1, 1));
        Text Exit = Text("Exit", buttonX + 60.0f, buttonY - 60, 1.0f, glm::vec3(1, 1, 1));

        uiText.push_back(Start);
        uiText.push_back(Exit);
    }

    void initializeButtons() {
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);
        startButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
        exitButton = Button(0.4f * windowWidth, 0.65f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
    }

    void renderText(const std::vector<Text>& renderingText) {
        for (const auto& text : renderingText) {
            textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
        }
    }

    void compileShaders() {
        //shader class to load shaders
        shader = new Shader("MainMenuShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
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

    void handleKeyboardInput(bool& inMenu) {
        static bool keyUpPressed = false; 
        static bool keyDownPressed = false;
        static bool enterPressed = false;


        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_RELEASE && keyUpPressed) {
            currentSelection = (currentSelection == 0) ? 1 : 0;
            keyUpPressed = false; 
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            keyUpPressed = true; 
        }


        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_RELEASE && keyDownPressed) {
            currentSelection = (currentSelection == 1) ? 0 : 1;
            keyDownPressed = false; 
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            keyDownPressed = true; 
        }

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && enterPressed) {
            if (currentSelection == 0) {
                inMenu = false; 
                audio.stopMusic();
            }
            if (currentSelection == 1) {
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);  
                audio.stopMusic();
            }
            enterPressed = false;
        }
        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            enterPressed = true; 
        }
    }
    void handleControllerInput(bool& inMenu) {
        static bool dpadUpPressed = false; 
        static bool dpadDownPressed = false;
        static bool aButtonPressed = false;

        if (!controller.isConnected()) return;


        if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP) && dpadUpPressed) {
            currentSelection = (currentSelection == 0) ? 1 : 0;
            dpadUpPressed = false;  
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP)) {
            dpadUpPressed = true; 
        }


        if (!controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN) && dpadDownPressed) {
            currentSelection = (currentSelection == 1) ? 0 : 1;
            dpadDownPressed = false;  
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN)) {
            dpadDownPressed = true;  
        }


        if (!controller.isButtonPressed(XINPUT_GAMEPAD_A) && aButtonPressed) {
            if (currentSelection == 0) {
                inMenu = false; 
                audio.stopMusic();
            }
            if (currentSelection == 1) {
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);  
                audio.stopMusic();
            }
            aButtonPressed = false;  
        }
        if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
            aButtonPressed = true; 
        }
    }


};

