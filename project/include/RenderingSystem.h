#pragma once

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Windowing.h"
#include "Entity.h"
#include "TTF.h"
#include "GameState.h"
#include "Text.h"

#include "Skybox.h"

class RenderingSystem {
public:
    RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer, GameState& gameState);

    void updateRenderer(
		std::vector<Model>& sceneModels,
		const std::vector<Text>& uiText,
        Skybox& skybox
    );

    void renderMinimap(Shader& minimapShader, Camera& minimapCam, int player);

private:
    Shader& shader;
    Camera& camera;
    Windowing& window;
    TTF& textRenderer;
    GameState& gState;

    void setShaderUniforms(Shader* shader);

    glm::mat4 createModelWithTransformations(const Entity* entity, const bool minimapRender);

    void renderEntities(const std::vector<Entity>& entities, Camera& cam, bool minimapRender = false);
    void renderText(const std::vector<Text>& renderingText);
    void renderSkybox(Skybox& skybox);
    void renderScene(std::vector<Model>& sceneModels);
    void updateFuelBar(Model& fuelBar, float fuelLevel);
};
