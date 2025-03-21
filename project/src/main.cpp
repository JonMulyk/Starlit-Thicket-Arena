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

#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Texture.h"
#include "Windowing.h"
#include <MainMenu.h>
#include <LevelSelect.h>



int main() {
    GameState gState;
    InitManager::initGLFW();

    // Model Setup
    std::vector<float> verts, coord;
    InitManager::getCube(verts, coord);
    Windowing window(1200, 1000);
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
    

    Shader shader("basicShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Shader lightingShader("lightingShader", "project/assets/shaders/lightingShader.vert", "project/assets/shaders/lightingShader.frag");
    Texture container("project/assets/textures/container.jpg", true);
    Texture gold("project/assets/textures/gold.jpg", true);
    Texture neon("project/assets/textures/neon.jpg", true);
    Texture fire("project/assets/textures/fire.jpg", true);
    Model cube(lightingShader, container, verts, verts, coord);
    Model redBrick(lightingShader, gold, "project/assets/models/box.obj");
    Model trail(lightingShader, fire, "project/assets/models/Trail.obj");
    Model tireModel(lightingShader, "project/assets/models/tire1/tire1.obj");
    Model secondCar(shader, "project/assets/models/bike/Futuristic_Car_2.1_obj.obj");
    std::vector<Model> sceneModels;
    GameStateEnum gameState = GameStateEnum::MENU;
    Command command;
    Command controllerCommand;
    TimeSeconds timer;
    Shader skyboxShader("project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Skybox skybox("project/assets/textures/skybox/", skyboxShader);


    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    Model groundPlaneModel2(sceneShader, gold, "project/assets/models/reallySquareArena.obj");
    Model groundPlaneModel3(sceneShader, fire, "project/assets/models/reallySquareArena.obj");
    Camera camera(gState, timer);
    UIManager uiManager(window.getWidth(), window.getHeight());
    int selectedLevel = -1;
    RenderingSystem renderer(shader, camera, window, arial, gState);
    const double roundDuration = 20;

    bool isAudioInitialized = false;
    Input input(window, camera, timer, command);
    Controller controller1(1, camera, controllerCommand);
    AudioSystem audio;

    MainMenu menu(window, arial, controller1);
    LevelSelectMenu levelSelectMenu(window, arial, controller1);

    while (!window.shouldClose()) {
        if (gameState == GameStateEnum::MENU) {
            bool startGame = false;
            while (!window.shouldClose() && !startGame) {
                menu.displayMenu();
                if (window.shouldClose()) break;
                selectedLevel = levelSelectMenu.displayMenuLevel();
                if (selectedLevel != -1) startGame = true;

            }
            if (!startGame || window.shouldClose()) {
                return 0;
            }
            gameState = GameStateEnum::PLAYING;
        }

        // Game setup
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!controller1.isConnected()) {
            std::cout << "Controller one not connected" << std::endl;
            controllerCommand.brake = 0.0f;
            controllerCommand.throttle = 0.0f;
            controllerCommand.steer = 0.0f;
        }

        PhysicsSystem* physicsSystem = new PhysicsSystem(gState, trail, secondCar);

        audio.init(physicsSystem, &camera);

        //slected level states and stuff
        if (selectedLevel == 1)
        {
            sceneModels.push_back(groundPlaneModel);
        }
        if (selectedLevel == 2)
        {
            sceneModels.push_back(groundPlaneModel2);
        }
        if (selectedLevel == 3)
        {
            sceneModels.push_back(groundPlaneModel3);
        }

        physicsSystem->updateTransforms(gState.dynamicEntities);



        // Main Loop
        timer.reset();
        timer.advance();
        while (!window.shouldClose() && gameState == GameStateEnum::PLAYING) {
            window.clear();
            timer.tick();
            input.poll();
            controller1.Update();
            audio.update();


            //pause
            if (input.isKeyReleased(GLFW_KEY_P) || controller1.isButtonJustReleased(XINPUT_GAMEPAD_START)) {
                gameState = GameStateEnum::PAUSE;
                timer.stop();
                audio.pauseMusic();
            }

            while (gameState == GameStateEnum::PAUSE) {
                input.poll();
                controller1.Update(); 

                if (input.isKeyReleased(GLFW_KEY_P) || controller1.isButtonJustReleased(XINPUT_GAMEPAD_START)) {
                    gameState = GameStateEnum::PLAYING;
                    timer.resume();
                    audio.resumeSounds();
                }
            }


            // Update physics
            if (gameState == GameStateEnum::PLAYING) {
                while (timer.getAccumultor() > 5 && timer.getAccumultor() >= timer.dt) {
                    physicsSystem->stepPhysics(timer.dt, command, controllerCommand);
                    physicsSystem->updatePhysics(timer.dt);
                    timer.advance();
                }
            }

            uiManager.updateUIText(timer, roundDuration, gState.getScore());
            renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
            glfwSwapBuffers(window);

            // Check for round end
            if (timer.getElapsedTime() >= roundDuration) {
                gameState = GameStateEnum::RESET;
            }
        }

        //reset
        if (gameState == GameStateEnum::RESET) {
            gameState = GameStateEnum::MENU;
            gState.dynamicEntities.clear();
            gState.staticEntities.clear();
            sceneModels.clear();

            gState.reset();
            audio.stopMusic(); 
            timer.reset();
            delete physicsSystem;
            //delete audio;

        }
    }
    return 0;
}