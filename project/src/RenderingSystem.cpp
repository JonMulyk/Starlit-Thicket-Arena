#include "RenderingSystem.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

RenderingSystem::RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer, GameState& gameState)
    : shader(shader), camera(camera), window(window), textRenderer(textRenderer), gState(gameState) {}

void RenderingSystem::setShaderUniforms(Shader* shader)
{
    if(shader->getName() == "basicShader")
    {
		shader->setFloat("repeats", 1.0f);
    }
    else if(shader->getName() == "lightingShader")
    {
        shader->setVec3("viewPos", camera.getPosition());
        shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader->setVec3("light.direction", glm::vec3(-0.5f, -1.0f, -0.5f));
        shader->setVec3("light.ambient", glm::vec3(0.45f, 0.45f, 0.45f));
        shader->setVec3("light.diffuse", glm::vec3(0.65f, 0.65f, 0.65f));
        shader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

glm::mat4 RenderingSystem::createModelWithTransformations(const Entity* entity, const bool minimapRender)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, entity->transform->pos);
	model *= glm::mat4_cast(entity->transform->rot);
    
    // minimap
	if (minimapRender)
	{
        // cars
		if (entity->name == "playerCar" || entity->name == "aiCar")
		{
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, entity->transform->scale * glm::vec3(1.9f, 1.0f, 1.9f));

            return model;
		}

        // trails
		if (entity->name == "playerVehicle" || entity->name == "vehicle1" || entity->name == "vehicle2" || entity->name == "vehicle3")
		{
			model = glm::scale(model, entity->transform->scale * glm::vec3(1.5f, 1.0f, 1.5f));
            return model;
		}
        
        // everything else
		model = glm::scale(model, entity->transform->scale);
		return model;
	}
    
    // non-minimap 
    // cars
	if (entity->name == "playerCar" || entity->name == "aiCar") {
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, entity->transform->scale * glm::vec3(0.5f, 1.0f, 0.4f));

        return model;
	}

    // everything else
	model = glm::scale(model, entity->transform->scale);

    return model;
}

void RenderingSystem::renderEntities(const std::vector<Entity>& entities, Camera& cam, bool minimapRender) 
{
	std::unordered_map<Shader*, std::vector<const Entity*>> shaderBatches;
    
    // optimiazation by batching entities that have the same shader together
	for (const auto& entity : entities)
	{
        shaderBatches[&entity.model->getShader()].push_back(&entity);
	}

	for (auto it = shaderBatches.begin(); it != shaderBatches.end(); ++it)
	{
        // Shader
		Shader* shaderPtr = it->first;
        // Grouped entities that share the same shader
		std::vector<const Entity*>& entityBatch = it->second;

		shaderPtr->use();
        cam.updateProjectionView(*shaderPtr, window.getWidth(), window.getHeight());
		setShaderUniforms(shaderPtr);

		for (const Entity* entity : entityBatch)
		{
            glm::mat4 model = createModelWithTransformations(entity, minimapRender);

			shaderPtr->setMat4("model", model);
            entity->model->draw(entity->name);
		}
	}
}

/*
void RenderingSystem::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    textRenderer.render(text, x, y, scale, color);
}
*/

void RenderingSystem::renderText(const std::vector<Text>& renderingText) {
    for (const auto& text : renderingText)
    {
		textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor(), text.getTextPosition());
    }
}

void RenderingSystem::renderScene(std::vector<Model>& sceneModels)
{
    sceneModels[0].getShader().use();
    this->camera.updateProjectionView(sceneModels[0].getShader(), window.getWidth(), window.getHeight());
    shader.setFloat("repeats", 4);


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(23.0f, 1.0f, 23.0f)); // Scale the ground
    model = glm::translate(model, glm::vec3(4.5f, 0.0f, 4.5f)); // translate ground
    //model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 180 degrees around X-axis
    sceneModels[0].getShader().setMat4("model", model);

    //sceneModels[0].draw();
}

void RenderingSystem::renderSkybox(Skybox& skybox)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skybox.getSkyboxShader().use();
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	skybox.getSkyboxShader().setMat4("view", view);
	//skyboxShader.setMat4("projection", projection);
    this->camera.updateProjectionView(skybox.getSkyboxShader(), window.getWidth(), window.getHeight());
    

	// skybox cube
	glBindVertexArray(skybox.getSkyboxVAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapTexture());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}


void RenderingSystem::updateRenderer(
    std::vector<Model>& sceneModels,
    const std::vector<Text>& uiText,
    Skybox& skybox
)
{
    glViewport(0, 0, window.getWidth(), window.getHeight());

	// Render Entities & Text
    this->renderSkybox(skybox);
    this->renderEntities(gState.dynamicEntities, this->camera);
    this->renderEntities(gState.staticEntities, this->camera);
    this->renderScene(sceneModels); // needs to be before any texture binds, otherwise it will take on those
    this->renderText(uiText);
	//this->renderText(textToDisplay, 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f));

}

void RenderingSystem::renderMinimap(Shader& minimapShader, Camera& minimapCam)
{
    minimapShader.use();

    int minimapWidth = window.getWidth() / 4.5;
    int minimapHeight = window.getHeight() / 4.5;
    int xOffset = -10;
    int yOffsetY = -10;
    
    glViewport(
        (window.getWidth() - minimapWidth) + xOffset,         // x position
		(window.getHeight() - minimapHeight) + yOffsetY,      // y position
        minimapWidth,                                         // width
        minimapHeight                                         // height
    );
    
    this->renderEntities(gState.dynamicEntities, minimapCam, true);
    this->renderEntities(gState.staticEntities, minimapCam, true);
    
    // reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
}
