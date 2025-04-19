#include "TransparentBoxRenderer.h"
#include <glad/glad.h> // Or <GL/glew.h> depending on your setup

TransparentBoxRenderer::TransparentBoxRenderer() {
    boxShader = new Shader("project/assets/shaders/transparentBoxShader.vert", "project/assets/shaders/transparentBoxShader.frag");
}

TransparentBoxRenderer::~TransparentBoxRenderer() {
    delete boxShader;
}

void TransparentBoxRenderer::draw(float x, float y, float width, float height, float alpha) {
    boxShader->use();
    boxShader->setBool("useTexture", false);
    boxShader->setBool("useSolidColor", false);
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
