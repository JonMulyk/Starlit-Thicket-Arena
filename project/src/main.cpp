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
    Model tireModel = Model(lightingShader, "project/assets/models/tire1/tire1.obj");
    Model Gtrail(lightingShader, "project/assets/models/Gtree/GTree.obj");
    Model Btrail(lightingShader, "project/assets/models/Btree/BTree.obj");
    Model Rtrail(lightingShader, "project/assets/models/Rtree/RTree.obj");
    Model Ytrail(lightingShader, "project/assets/models/Ytree/YTree.obj");
    std::vector<Model> trails = { Gtrail, Btrail, Rtrail, Ytrail };
    //Model secondCar(lightingShader, gold, "project/assets/models/box.obj");
    Model secondCar(shader, "project/assets/models/bike/Futuristic_Car_2.1_obj.obj");
    PhysicsSystem* physicsSystem = new PhysicsSystem(gState, trails, secondCar);

    AudioSystem audio;
	AudioSystem* audioPtr = &audio;
    audio.init(physicsSystem, &camera);

    // Create Rendering System
    RenderingSystem renderer(shader, camera, window, arial, gState);

    // Static scene data
    std::vector<Model> sceneModels;

    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    sceneModels.push_back(groundPlaneModel);


    physicsSystem->updateTransforms(gState.dynamicEntities);

    // Text Rendering Setup
    UIManager uiManager(window.getWidth(), window.getHeight());
    
    const double roundDuration = (5.0 * 60.0);

    //SKYBOX
    Shader skyboxShader("project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
    Skybox skybox("project/assets/textures/skybox/", skyboxShader);

    // Main Loop
    timer.advance();
    while (!window.shouldClose()) {
        window.clear();
        timer.tick();
        input.poll();
        controller1.Update();
		audio.update();

        // Update physics
        while (timer.getAccumultor() > 5  && timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command, controllerCommand);

            physicsSystem->updatePhysics(timer.dt);
            timer.advance();
        }

		//renderer.renderScene(sceneModels);
    
        uiManager.updateUIText(timer, roundDuration, gState.getScore());
        renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);

        glfwSwapBuffers(window);
    }
    for (int i = 0; i < gState.staticEntities.size(); i++) {
       std::cout << gState.staticEntities[i].name << " " << gState.staticEntities[i].model.directory << "\n";
    }
    // playerVehicle project/assets/models/Gtree
    // vehicle1 project/assets/models/Btree
    // vehicle2 project/assets/models/Rtree
    // vehicle3 project/assets/models/Ytree
    //gState.gMap.printGraph();
    return 0;
}