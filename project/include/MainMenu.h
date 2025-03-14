#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"
#include <Button.h>


class MainMenu {
public:
    MainMenu(Windowing& window, TTF& textRenderer)
        : window(window), textRenderer(textRenderer),
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
    }

    void displayMenu() {
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        bool inMenu = true;
        while (inMenu && !window.shouldClose()) {
            window.clear();
            renderMenu();
            window.swapBuffer();
            glfwPollEvents();

            if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);
                glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

                if (startButton.isClicked(xpos, ypos)) {
                    inMenu = false;
                    //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                if (exitButton.isClicked(xpos, ypos)) {
                    glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                }
            }
        }
    }

private:
    std::vector<Text> uiText;
    Windowing& window;
    Shader* shader;
    unsigned int backgroundTexture;
    TTF& textRenderer;
    int windowWidth, windowHeight;
    Button startButton, exitButton;

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();

        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        startButton = Button(0.4f * windowWidth, 0.5f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));
        exitButton = Button(0.4f * windowWidth, 0.65f * windowHeight, 0.2f * windowWidth, 0.0625f * windowHeight, glm::vec3(1, 0, 0));

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
};

