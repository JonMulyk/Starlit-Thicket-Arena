#pragma once

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Windowing.h"
#include "Entity.h"
#include "TTF.h"
#include "GameState.h"
#include "Text.h"

class RenderingSystem {
public:
    RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer, GameState& gameState);

    void updateRenderer(
		std::vector<Model>& sceneModels,
		const std::vector<Text>& uiText
    );

private:
    Shader& shader;
    Camera& camera;
    Windowing& window;
    TTF& textRenderer;
    GameState& gState;

    void updateProjectionView(Shader& viewShader);
    void setShaderUniforms(Shader* shader);
    void renderEntities(const std::vector<Entity>& entities);
    void renderText(const std::vector<Text>& renderingText);
    void renderScene(std::vector<Model>& sceneModels);
};
