#pragma once

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Windowing.h"
#include "Entity.h"
#include "TTF.h"
#include "GameState.h"
#include "UIManager.h"
#include "FuelBar.h"

#include "Skybox.h"

class RenderingSystem {
public:
    RenderingSystem(Shader& shader, Camera& camera, Windowing& window, TTF& textRenderer, GameState& gameState);

    void updateRenderer(
        std::vector<Model>& sceneModels,
        UIManager& uiManager,
        Skybox& skybox,
        uint16_t playerID
    );

    void renderMinimap(Shader& minimapShader, Camera& minimapCam, int player);

private:
    Shader& shader;
    Camera& camera;
    Windowing& window;
    TTF& textRenderer;
    GameState& gState;

    void renderSkybox(Skybox& skybox);
    void renderEntities(const std::vector<Entity>& entities, Camera& cam, bool minimapRender = false);
    void renderScene(std::vector<Model>& sceneModels);
    void renderGroundPlane(Model& groundPlane);
    void renderFuelBar(std::vector<FuelBar>& fuelBars, uint16_t playerID);
    void renderText(const std::vector<Text>& renderingText);

    void updateFuelBar(Model& fuelBar, float fuelLevel);
    void updateFuelBarSize(Model& fuelBar, float fuelLevel);
    void updateFuelBarColor(Model& fuelBar, float fuelLevel);

    void setShaderUniforms(Shader* shader);
    void colorVehicle(Shader* shader, const std::string name);
    glm::mat4 createModelWithTransformations(const Entity* entity, const bool minimapRender);
};
