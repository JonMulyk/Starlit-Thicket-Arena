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

    void renderEntities(const std::vector<Entity>& entities);

    void renderText(const std::vector<Text>& renderingText);
    //void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void updateProjectionView(Shader& viewShader);
    void renderScene(std::vector<Model>& sceneModels);
};
