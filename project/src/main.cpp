#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>

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

int main() {
    GameState gState;
    InitManager::initGLFW();
    Command command;
    TimeSeconds timer;
    Camera camera(gState);
    Windowing window(1200, 1000);

    Input input(window, camera, timer, command);
    Shader shader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
    Texture container("project/assets/textures/container.jpg", true);
    Texture gold("project/assets/textures/gold.jpg", true);
    Texture neon("project/assets/textures/neon.jpg", true);
    Texture fire("project/assets/textures/fire.jpg", true);

    // Model Setup
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Model cube(shader, container, verts, verts, coord);
    Model redBrick(shader, gold, "project/assets/models/box.obj");
    Model trail(shader, fire, "project/assets/models/Trail.obj");

    PhysicsSystem* physicsSystem = new PhysicsSystem(gState, trail);

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
            gState.dynamicEntities.emplace_back(Entity("box", cube, physicsSystem->getTransformAt(counter++)));
        }
    }
    delete(boxGeom);

    physicsSystem->updateTransforms(gState.dynamicEntities);

    //controller input
    Controller controller1(1);
    if (!controller1.isConnected()) { std::cout << "Controller one not connected" << std::endl; }

    // Main Loop
    while (!window.shouldClose()) {
        if (controller1.Update()) {           
            //std::cout << "Left Stick " << "X: " << controller1.leftStickX << ", Y: " << controller1.leftStickY << std::endl;
            //std::cout << "Right Stick " << "X: " << controller1.rightStickX << ", Y: " << controller1.rightStickY << std::endl;
            //UINT button = controller1.buttodID();
            //std::cout << button << std::endl;
            //std::cout << "Trigger Values: " << controller1.leftTrigger << ", " << controller1.rightTrigger << std::endl;
            //std::cout << "Button values: " << controller1.isButtonPressed(XINPUT_GAMEPAD_A);
            //controller1.Vibrate(20000, 20000);
            //controller1.resetVibration();
        }

        window.clear();
        timer.tick();
        input.poll();

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command);
            physicsSystem->updatePhysics(timer.dt);
            timer.advance();
        }

		//renderer.renderScene(sceneModels);
        renderer.updateRenderer(sceneModels, "FPS: " + std::to_string(timer.getFPS()));

        glfwSwapBuffers(window);
    }

    return 0;
}
