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
#include <Vehicle.h>

int main() {
    GameState gState;
    InitManager::initGLFW();
    Command command;
	Command controllerCommand;
    Command commandAI;
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



    // Model Setups
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Model cube(lightingShader, container, verts, verts, coord);
    Model redBrick(lightingShader, gold, "project/assets/models/box.obj");
    Model trail(lightingShader, fire, "project/assets/models/Trail.obj");
    Model tireModel = Model(lightingShader, "project/assets/models/tire1/tire1.obj");
    //Model secondCar(lightingShader, gold, "project/assets/models/box.obj");
    Model secondCar(shader, "project/assets/models/bike/Futuristic_Car_2.1_obj.obj");
    PhysicsSystem* physicsSystem = new PhysicsSystem(gState, trail);

    // Create Rendering System
    RenderingSystem renderer(shader, camera, window, arial, gState);

    // Entity setup
    gState.dynamicEntities.emplace_back("car", secondCar, physicsSystem->getTransformAt(0));
    gState.dynamicEntities.emplace_back("car", secondCar, physicsSystem->getTransformAt(1));

    // Static scene data
    std::vector<Model> sceneModels;

    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    sceneModels.push_back(groundPlaneModel);

    // PhysX item setup
    float halfLen = 0.5f;
    MaterialProp matProps = { 0.5f, 0.5f, 0.6f };
    physx::PxBoxGeometry* boxGeom = new physx::PxBoxGeometry(halfLen, halfLen, halfLen);

    //int size = 5;
    //int counter = 1;
    //for (unsigned int i = 0; i < size; i++) {
     //   for (unsigned int j = 0; j < size - i; j++) {
     //       physx::PxTransform localTran(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 - 1), 0) * halfLen);
     //       physicsSystem->addItem(matProps, boxGeom, localTran, 10.f);
     //       gState.dynamicEntities.emplace_back(Entity("box", tireModel, physicsSystem->getTransformAt(counter++)));
     //   }
    //}
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

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command, controllerCommand);

            physicsSystem->updatePhysics(timer.dt);
            timer.advance();
        }

        Command aiCommand1;
        aiCommand1.throttle = 0.5f;
        aiCommand1.brake = 0.0f;   //no braking
        aiCommand1.steer = -0.5f;   //right is negative, left is positive floats
        physicsSystem->setVehicleCommand(1, aiCommand1);

		//renderer.renderScene(sceneModels);
    
        uiManager.updateUIText(timer, roundDuration, gState.getScore());
        renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);

        glfwSwapBuffers(window);
    }

    gState.gMap.printGraph();
    gState.gMap.printGraph(gState.gMap.aStar(0, 0, GRID_SIZE - 1, GRID_SIZE - 1));
    return 0;
}
