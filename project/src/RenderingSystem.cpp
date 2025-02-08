#include "RenderingSystem.h"
#include <glm/gtc/matrix_transform.hpp>

RenderingSystem::RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer)
    : shader(shader), camera(camera), window(window), textRenderer(textRenderer) {}

void RenderingSystem::updateProjectionView() {
    shader.use();

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getZoom()),
        float(window.getWidth()) / float(window.getHeight()),
        0.1f, 100.0f
    );
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);
}

void RenderingSystem::renderEntities(const std::vector<Entity>& entities) {
    shader.use();
    updateProjectionView();

    for (const auto& entity : entities) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, entity.transform->pos);
        model *= glm::mat4_cast(entity.transform->rot);
        model = glm::scale(model, glm::vec3(1.0f));

        shader.setMat4("model", model);
        entity.model.Draw(shader);
    }
}

void RenderingSystem::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    textRenderer.render(text, x, y, scale, color);
}
