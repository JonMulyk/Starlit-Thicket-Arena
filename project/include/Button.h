#pragma once
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"
#include "TTF.h"

class Button {
public:
    Button(float x, float y, float width, float height, glm::vec3 color)
        : x(x), y(y), width(width), height(height), color(color) {}

    void draw(Shader* shader, int windowWidth, int windowHeight) {
        auto convertX = [windowWidth](float xPos) -> float {
            return (2.0f * xPos / windowWidth) - 1.0f;
        };
        auto convertY = [windowHeight](float yPos) -> float {
            return 1.0f - (2.0f * yPos / windowHeight);
        };

        float vertices[] = {
            convertX(x), convertY(y), 0.0f, 0.0f,
            convertX(x), convertY(y + height), 0.0f, 0.0f,
            convertX(x + width), convertY(y + height), 0.0f, 0.0f,
            convertX(x), convertY(y), 0.0f, 0.0f,
            convertX(x + width), convertY(y + height), 0.0f, 0.0f,
            convertX(x + width), convertY(y), 0.0f, 0.0f
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
        shader->setInt("useTexture", 0);
        shader->setVec3("solidColor", color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    bool isClicked(double mouseX, double mouseY) {
        return mouseX >= x && mouseX <= x + width &&
            mouseY >= y && mouseY <= y + height;
    }

private:
    float x, y, width, height;
    glm::vec3 color;
};
