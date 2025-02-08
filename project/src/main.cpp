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

    physicsSystem->updateTransforms(entityList);


    //trail spawning TODO: 
    /*
    Put all the stuff in car.cpp and car.h thing
    */
    int maxObjects = 20;  //adjust any of these values to test stuff
    int objectsSpawned = 0; 

    float spawnInterval = 10.0f;
    float timeSinceLastSpawn = 0.0f;

    // Main Loop
    while (!window.shouldClose()) {
        window.clear();
        timer.tick();
        input.poll();

        
        timeSinceLastSpawn += timer.dt;

       
        if (timeSinceLastSpawn >= spawnInterval && objectsSpawned < maxObjects) {

            physx::PxVec3 carPosition = physicsSystem->getCarPosition();

            float offsetBehindCar = 0.0f; 
            physx::PxTransform spawnTransform(carPosition + physx::PxVec3(0, 0, 0.0f)); 


            MaterialProp matProps = { 0.5f, 0.5f, 0.6f };
            physx::PxBoxGeometry* boxGeom = new physx::PxBoxGeometry(halfLen, halfLen, halfLen);
            physicsSystem->addItem(matProps, boxGeom, spawnTransform, 10.f);
            entityList.emplace_back(Entity("box", cube, physicsSystem->getTransformAt(counter++)));

            delete(boxGeom);

            objectsSpawned++; 
            timeSinceLastSpawn = 0.0f;
        }

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
            physicsSystem->stepPhysics(timer.dt, command);
            physicsSystem->updatePhysics(timer.dt, entityList);
            timer.advance();
        }

        // Render Entities & Text
        renderer.renderEntities(entityList);
        renderer.renderText("FPS: " + std::to_string(timer.getFPS()), 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f));

        glfwSwapBuffers(window);
    }


    return 0;
}
