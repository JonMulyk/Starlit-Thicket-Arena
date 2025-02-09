#include "RenderingSystem.h"
#include <glm/gtc/matrix_transform.hpp>

RenderingSystem::RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer)
    : shader(shader), camera(camera), window(window), textRenderer(textRenderer) {}

void RenderingSystem::updateProjectionView(Shader &viewShader) {
    viewShader.use();

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getZoom()),
        float(window.getWidth()) / float(window.getHeight()),
        0.1f, 100.0f
    );
    viewShader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    viewShader.setMat4("view", view);
}

void RenderingSystem::renderEntities(const std::vector<Entity>& entities) {
    shader.use();
    updateProjectionView(shader);

    for (const auto& entity : entities) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, entity.transform->pos);
        model *= glm::mat4_cast(entity.transform->rot);
        model = glm::scale(model, glm::vec3(1.0f));

        shader.setMat4("model", model);
        entity.model.draw();
    }
}

void RenderingSystem::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    textRenderer.render(text, x, y, scale, color);
}

void RenderingSystem::renderScene(Shader &gpShader, Model& groundPlaneModel)
{
    gpShader.use();
    updateProjectionView(gpShader); // set the correct matrices

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f)); // Move ground down 
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 180 degrees around X-axis
    model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f)); // Scale the ground
    gpShader.setMat4("model", model);

    groundPlaneModel.draw();
}


void RenderingSystem::updateRenderer(const std::vector<Entity>& entities, std::string textToDisplay)
{

	// Render Entities & Text
	this->renderEntities(entities);
	this->renderText(textToDisplay, 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f));


}
