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

int main() {
    InitManager::initGLFW();

    TimeSeconds timer;
    Camera camera;
    Windowing window(1200, 1000);

    Input input(window, camera, timer);
    Shader shader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
    Texture texture1("project/assets/textures/container.jpg", true);
    Texture texture2("project/assets/textures/wall.jpg", true);

    PhysicsSystem* physicsSystem = new PhysicsSystem();

    // Create Rendering System
    RenderingSystem renderer(shader, camera, window, arial);

    // Model Setup
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Model m2(shader, texture2, verts, verts, coord);
    Model m1(shader, texture2, "project/assets/models/GTree.obj");


    // PhysX item setup
    float halfLen = 0.5f;
    MaterialProp matProps = { 0.5f, 0.5f, 0.6f };
    physx::PxBoxGeometry* boxGeom = new physx::PxBoxGeometry(halfLen, halfLen, halfLen);

    // Entity setup
    std::vector<Entity> entityList;

    int size = 5;
    int counter = 0;
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size - i; j++) {
            physx::PxTransform localTran(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 - 1), 0) * halfLen);
            physicsSystem->addItem(matProps, boxGeom, localTran, 10.f);
            entityList.emplace_back(Entity("box", m2, physicsSystem->getTransformAt(counter++)));
        }
    }
    delete(boxGeom);

    physicsSystem->updateTransforms(entityList);

    // Main Loop
    while (!window.shouldClose()) {
        window.clear();
        timer.tick();
        input.poll();

        // Update physics
        while (timer.getAccumultor() >= timer.dt) {
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
