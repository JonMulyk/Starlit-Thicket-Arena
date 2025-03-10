#include <GLFW/glfw3.h>
#include "stb_image.h"

class MainMenu {
public:
    MainMenu(Windowing& window) : window(window) {
        compileShaders();
        loadBackgroundTexture();
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
                
                // Get current window size
                int windowWidth, windowHeight;
                glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

                //button bounds
                float startX = 0.5f * windowWidth - (0.1f * windowWidth); 
                float startY = 0.5f * windowHeight - (0.03125f * windowHeight); 
                float startWidth = 0.2f * windowWidth;  
                float startHeight = 0.0625f * windowHeight; 
                if (isButtonClicked(xpos, ypos, startX, startY, startWidth, startHeight)) {
                    inMenu = false;
                    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }

                //button bounds for exit game
                float exitX = 0.5f * windowWidth - (0.1f * windowWidth);
                float exitY = startY + startHeight + (0.15f * windowHeight); 
                float exitWidth = 0.2f * windowWidth;
                float exitHeight = 0.0625f * windowHeight; 
                if (isButtonClicked(xpos, ypos, exitX, exitY, exitWidth, exitHeight)) {
                    glfwSetWindowShouldClose(window.getGLFWwindow(), true);
                }
            }
        }
    }

private:
    Windowing& window;
    unsigned int shaderProgram;
    unsigned int backgroundTexture;

    void renderMenu() {
        glDisable(GL_DEPTH_TEST);
        drawBackground();


        int windowWidth, windowHeight;
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);


        float startX = 0.5f * windowWidth - (0.1f * windowWidth);
        float startY = 0.5f * windowHeight - (0.03125f * windowHeight); 
        float startWidth = 0.2f * windowWidth;    
        float startHeight = 0.0625f * windowHeight; 
        drawButton(startX, startY, startWidth, startHeight);

        float exitX = 0.5f * windowWidth - (0.1f * windowWidth); 
        float exitY = startY + startHeight + (0.15f * windowHeight);
        float exitWidth = 0.2f * windowWidth;   
        float exitHeight = 0.0625f * windowHeight; 
        drawButton(exitX, exitY, exitWidth, exitHeight);
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

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void drawButton(int x, int y, int width, int height) {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

        auto convertX = [windowWidth](float xPos) -> float {
            return (2.0f * xPos / windowWidth) - 1.0f;
        };
        auto convertY = [windowHeight](float yPos) -> float {
            return 1.0f - (2.0f * yPos / windowHeight);
        };

        float vertices[] = {
            convertX(x),           convertY(y),           0.0f, 0.0f,  // top-left
            convertX(x),           convertY(y + height),  0.0f, 0.0f,  // bottom-left
            convertX(x + width),   convertY(y + height),  0.0f, 0.0f,  // bottom-right
            convertX(x),           convertY(y),           0.0f, 0.0f,  // top-left
            convertX(x + width),   convertY(y + height),  0.0f, 0.0f,  // bottom-right
            convertX(x + width),   convertY(y),           0.0f, 0.0f   // top-right
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

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);
        glUniform3f(glGetUniformLocation(shaderProgram, "solidColor"), 1.0f, 0.0f, 0.0f); // Red color
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    bool isButtonClicked(double mouseX, double mouseY, int btnX, int btnY, int btnWidth, int btnHeight) {
        return mouseX >= btnX && mouseX <= btnX + btnWidth &&
               mouseY >= btnY && mouseY <= btnY + btnHeight;
    }

    void compileShaders() {
        const char* vertexShaderSource = R"(
            #version 330 core
            layout(location = 0) in vec2 aPos;
            layout(location = 1) in vec2 aTexCoord;
            out vec2 TexCoord;
            void main() {
                gl_Position = vec4(aPos, 0.0, 1.0);
                TexCoord = aTexCoord;
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 330 core
            in vec2 TexCoord;
            out vec4 FragColor;
            uniform sampler2D texture1;
            uniform bool useTexture;
            uniform vec3 solidColor;
            void main() {
                if (useTexture) {
                    FragColor = texture(texture1, TexCoord);
                } else {
                    FragColor = vec4(solidColor, 1.0);
                }
            }
        )";

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
};