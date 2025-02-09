#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>

#include "PxPhysicsAPI.h"
#include "TimeSeconds.h"
#include "InitManager.h"
#include "Windowing.h"
#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "TTF.h"
#include "Model.h"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "Camera.h"
#include "RenderingSystem.h"

int main() {
    InitManager::initGLFW();
    Command command;
    TimeSeconds timer;
    Camera camera;
    Windowing window(1200, 1000);

    Input input(window, camera, timer, command);
    Shader shader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
    Texture container("project/assets/textures/container.jpg", true);
    Texture brickWall("project/assets/textures/wall.jpg", true);

    PhysicsSystem* physicsSystem = new PhysicsSystem();

    // Create Rendering System
    RenderingSystem renderer(shader, camera, window, arial);

    // Model Setup
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Model cube(shader, container, verts, verts, coord);
    Model redBrick(shader, brickWall, "project/assets/models/redBrick.obj");


    // Entity setup
    std::vector<Entity> entityList;
    entityList.emplace_back("car", redBrick, physicsSystem->getTransformAt(0));

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
            entityList.emplace_back(Entity("box", cube, physicsSystem->getTransformAt(counter++)));
        }
    }
    delete(boxGeom);


    Shader shader2("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Texture tex("project/assets/textures/container.jpg", true);
    Model groundPlaneModel(shader2, tex, "project/assets/models/squareArena.obj");


    physicsSystem->updateTransforms(entityList);

    // Main Loop
    while (!window.shouldClose()) {
        window.clear();
        timer.tick();
        input.poll();

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command);
            physicsSystem->updatePhysics(timer.dt, entityList);
            timer.advance();
        }

		renderer.renderScene(shader2, groundPlaneModel);
        renderer.updateRenderer(entityList, "FPS: " + std::to_string(timer.getFPS()));

        glfwSwapBuffers(window);
    }

    return 0;
}
