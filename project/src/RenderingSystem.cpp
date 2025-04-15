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
		if (entity->name == "playerCar" || (entity->name == "aiCar1" || entity->name == "aiCar2" || entity->name == "aiCar3"))
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
	if (entity->name == "playerCar" || (entity->name == "aiCar1" || entity->name == "aiCar2" || entity->name == "aiCar3")) {
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
    for (int i = 0; i < sceneModels.size(); i++)
    {
        if(i == 0)
        {
			sceneModels[0].getShader().use();
			this->camera.updateProjectionView(sceneModels[0].getShader(), window.getWidth(), window.getHeight());
			shader.setFloat("repeats", 4.0f);


			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(23.0f, 1.0f, 23.0f)); // Scale the ground
			model = glm::translate(model, glm::vec3(4.5f, 0.0f, 4.5f)); // translate ground
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate 180 degrees around X-axis
			sceneModels[0].getShader().setMat4("model", model);

			sceneModels[0].draw();
        }
        else if (i == 1)
        {
			sceneModels[i].getShader().use();

            glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f); // left, right, bottom, top
            sceneModels[i].getShader().setMat4("projection", projection);

            updateFuelBar(sceneModels[i], gState.playerVehicle.fuel);

			sceneModels[i].draw();
        }
    }
}

void RenderingSystem::updateFuelBar(Model& fuelBar, float fuelLevel)
{
    float x = -0.9f;
    float y = -0.9f;
    float fullWidth = 0.4;
    float height = -0.05;
    float clamped = std::min(std::max(fuelLevel, 0.0f), 1.0f);
    float width = fullWidth * clamped;

    std::vector<float> vertices = {
		x, y, 0.0f,
		x + width, y, 0.0f,
		x + width, y + height, 0.0f,

		x, y, 0.0f,
		x + width, y + height, 0.0f,
		x, y + height, 0.0f
    };

    glm::vec4 color;

    float fuelLevelGreenMin = 0.999f;
    float fuelLevelYellowMin = 0.375f;
    float fuelLevelRedMin = 0.0f;

    glm::vec4 green = { 32.0f / 255.0f, 237.0f / 255.0f, 34.0f / 255.0f, 1.0f };
    glm::vec4 yellow = {226.0f / 255.0f, 182.0f / 255.0f, 26.0f / 255.0f, 1.0f};
    glm::vec4 red = { 255.0f / 255.0f, 21.0f / 255.0f, 15.0f / 255.0f, 1.0f };

    if (fuelLevel > fuelLevelGreenMin)
    {
        color = green;
    }
    else if (fuelLevel > fuelLevelYellowMin)
    {
        color = yellow;
    }
    else
    {
        color = red;
    }

    fuelBar.getShader().use();
    fuelBar.getShader().setVec4("barColor", color);

    fuelBar.updateVertices(vertices);
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
    //glViewport(0, 0, window.getWidth(), window.getHeight());

	// Render Entities & Text
    this->renderSkybox(skybox);
    this->renderEntities(gState.dynamicEntities, this->camera);
    this->renderEntities(gState.staticEntities, this->camera);
    this->renderScene(sceneModels); // needs to be before any texture binds, otherwise it will take on those
    this->renderText(uiText);
	//this->renderText(textToDisplay, 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f));

}

void RenderingSystem::renderMinimap(Shader& minimapShader, Camera& minimapCam, int player)
{
    // Retrieve current viewport dimensions.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);  // viewport: [x, y, width, height]
    int vpX = viewport[0], vpY = viewport[1];
    int vpWidth = viewport[2], vpHeight = viewport[3];

    // Define minimap size (1/4 of viewport) and position (top-right with an offset).
    int minimapWidth = vpWidth / 4;
    int minimapHeight = vpHeight / 4;
    int offset = 10;  // offset in pixels
    int minimapX = vpX + vpWidth - minimapWidth - offset;
    int minimapY = vpY + vpHeight - minimapHeight - offset;

    // Set the shader and the minimap viewport.
    minimapShader.use();
    glViewport(minimapX, minimapY, minimapWidth, minimapHeight);

    //background
	bool background = true;
    if (background) {
        glEnable(GL_SCISSOR_TEST);
        if (!gState.splitScreenEnabled && !gState.splitScreenEnabled4) glScissor(minimapX + 106, minimapY, minimapWidth / 2 + 30, minimapHeight);
        if (gState.splitScreenEnabled) glScissor(minimapX + 172, minimapY, minimapWidth / 4 + 15, minimapHeight);
        if (gState.splitScreenEnabled4) glScissor(minimapX + 53, minimapY, minimapWidth / 2 + 15, minimapHeight);
        GLfloat oldClearColor[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
        glClearColor(0.82f, 0.82f, 0.82f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
        glDisable(GL_SCISSOR_TEST);
    }
    // -----------------------------------------------------

    // Compute the minimap's aspect ratio.
    float minimapAspect = static_cast<float>(minimapWidth) / static_cast<float>(minimapHeight);
    float scale = 100.0f;
    glm::mat4 projection = glm::ortho(
        -scale * minimapAspect, scale * minimapAspect,
        -scale, scale,
        -1000.0f, 1000.0f
    );
    minimapShader.setMat4("projection", projection);

    // Get the original view matrix and then rotate it.
    glm::mat4 view = minimapCam.GetViewMatrix();
    float angle = glm::radians(90.0f * player);
    if (player == 1) angle = glm::radians(180.f);
    if (player == 3) angle = glm::radians(0.f);
    view = glm::rotate(view, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    minimapShader.setMat4("view", view);

    // Batch and render both dynamic and static entities using our custom matrices.
    std::unordered_map<Shader*, std::vector<const Entity*>> shaderBatches;
    for (const auto& entity : gState.dynamicEntities)
        shaderBatches[&entity.model->getShader()].push_back(&entity);
    for (const auto& entity : gState.staticEntities)
        shaderBatches[&entity.model->getShader()].push_back(&entity);

    for (auto it = shaderBatches.begin(); it != shaderBatches.end(); ++it) {
        Shader* shaderPtr = it->first;
        shaderPtr->use();
        shaderPtr->setMat4("projection", projection);
        shaderPtr->setMat4("view", view);
        setShaderUniforms(shaderPtr);
        for (const Entity* entity : it->second) {
            glm::mat4 model = createModelWithTransformations(entity, true);
            shaderPtr->setMat4("model", model);
            entity->model->draw(entity->name);
        }
    }

    glViewport(vpX, vpY, vpWidth, vpHeight);
}