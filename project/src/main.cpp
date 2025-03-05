#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>

#include <chrono>

#include "PxPhysicsAPI.h"
#include "TimeSeconds.h"
#include "InitManager.h"
#include "Windowing.h"
#include "Input.h"
#include "Controller.h"
#include "Shader.h"
#include "Texture.h"
#include "TTF.h"
#include "Model.h"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "Camera.h"
#include "RenderingSystem.h"
#include "GameState.h"
#include "UIManager.h"
#include "Skybox.h"
#include "AudioSystem.h"

int main() {
    GameState gState;
    InitManager::initGLFW();
    Command command;
	Command controllerCommand;
    TimeSeconds timer;
    Camera camera(gState, timer);
    Windowing window(1200, 1000);

    Input input(window, camera, timer, command);
    Controller controller1(1, camera, controllerCommand);
    if (!controller1.isConnected()) { 
        std::cout << "Controller one not connected" << std::endl; 
		controllerCommand.brake = 0.0f;
		controllerCommand.throttle = 0.0f;
		controllerCommand.steer = 0.0f;
    }

    Shader shader("basicShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Shader lightingShader("lightingShader", "project/assets/shaders/lightingShader.vert", "project/assets/shaders/lightingShader.frag");
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
    Texture container("project/assets/textures/container.jpg", true);
    Texture gold("project/assets/textures/gold.jpg", true);
    Texture neon("project/assets/textures/neon.jpg", true);
    Texture fire("project/assets/textures/fire.jpg", true);

    // Model Setup
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Model cube(lightingShader, container, verts, verts, coord);
    Model redBrick(lightingShader, gold, "project/assets/models/box.obj");
    Model trail(lightingShader, fire, "project/assets/models/Trail.obj");
    Model tireModel = Model(lightingShader, "project/assets/models/tire1/tire1.obj");

    PhysicsSystem* physicsSystem = new PhysicsSystem(gState, trail);

    AudioSystem audio;
    AudioSystem* audio_ptr = &audio;
    audio.init(physicsSystem);
    bool audioMove = false, first = true;

    // Create Rendering System
    RenderingSystem renderer(shader, camera, window, arial, gState);

    // Entity setup
    gState.dynamicEntities.emplace_back("car", redBrick, physicsSystem->getTransformAt(0));

    // Static scene data
    std::vector<Model> sceneModels;

    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    sceneModels.push_back(groundPlaneModel);

    // PhysX item setup
    float halfLen = 0.5f;
    MaterialProp matProps = { 0.5f, 0.5f, 0.6f };
    physx::PxBoxGeometry* boxGeom = new physx::PxBoxGeometry(halfLen, halfLen, halfLen);

    int size = 5;
    int counter = 1;
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size - i; j++) {
            physx::PxTransform localTran(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 - 1), 0) * halfLen);
            physicsSystem->addItem(matProps, boxGeom, localTran, 10.f);
            gState.dynamicEntities.emplace_back(Entity("box", tireModel, physicsSystem->getTransformAt(counter++)));
        }
    }
    delete(boxGeom);

    physicsSystem->updateTransforms(gState.dynamicEntities);

    // Text Rendering Setup
    UIManager uiManager(window.getWidth(), window.getHeight());
    
    const double roundDuration = (5.0 * 60.0);

    //SKYBOX
    Shader skyboxShader("project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
    Skybox skybox("project/assets/textures/skybox/", skyboxShader);

    // Main Loop
    while (!window.shouldClose()) {
        window.clear();
        timer.tick();
        input.poll();
        controller1.Update();
		audio.update();

        if (audioMove && first) {
            audio.startBattleMusic();
            audioMove = false;
			first = false;
        }

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command, controllerCommand, audioMove);
            physicsSystem->updatePhysics(timer.dt);
            timer.advance();
        }
    
        uiManager.updateUIText(timer, roundDuration, gState.getScore());
        renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);

        glfwSwapBuffers(window);
    }

	audio.shutdown();

    return 0;
}
