#pragma once

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
#include "DynamicCamera.h"
#include "StaticCamera.h"
#include "RenderingSystem.h"
#include "GameState.h"
#include "UIManager.h"
#include "Skybox.h"
#include "AudioSystem.h"
#include "MainMenu.h"
#include "LevelSelect.h"



int main() {
    GameState gState;
    InitManager::initGLFW();
    Command command;
	Command controllerCommand;
    TimeSeconds timer;
    DynamicCamera camera(gState, timer);
    //Camera camera(gState, timer, true, glm::vec3(0.0f, 250.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), -90.0f, -90.0f);

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
    Model Gtrail(lightingShader, "project/assets/models/Gtree/GTree.obj");
    Model Btrail(lightingShader, "project/assets/models/Btree/BTree.obj");
    Model Rtrail(lightingShader, "project/assets/models/Rtree/RTree.obj");
    Model Ytrail(lightingShader, "project/assets/models/Ytree/YTree.obj");
    Model tireModel(lightingShader, "project/assets/models/tire1/tire1.obj");
    Model secondCar(shader, "project/assets/models/bike/Futuristic_Car_2.1_obj.obj");
    std::vector<Model> sceneModels;
    GameStateEnum gameState = GameStateEnum::MENU;
    
    // Skybox
    Shader skyboxShader("project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Skybox skybox("project/assets/textures/skybox/", skyboxShader);

    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    UIManager uiManager(window.getWidth(), window.getHeight());
    int selectedLevel = -1;
    RenderingSystem renderer(shader, camera, window, arial, gState);
    const double roundDuration = 20;

    bool isAudioInitialized = false;
    AudioSystem audio;

    MainMenu menu(window, arial, controller1);
    LevelSelectMenu levelSelectMenu(window, arial, controller1);


    std::vector<Model> models = { Gtrail, Btrail, Rtrail, Ytrail, secondCar, cube};

    // Minimap 
    Shader minimapShader("minimapShader", "project/assets/shaders/minimapShader.vert", "project/assets/shaders/minimapShader.frag");
    StaticCamera minimapCamera(timer, glm::vec3(0.0f, 250.0f, 0.0f), 
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Number of players playing for scoreboard 
    uint16_t numberOfPlayers = 1;
    uint16_t numberOfAiCars = 3;

    // Main Loop
    //timer.advance();
    while (!window.shouldClose()) {
        if (gameState == GameStateEnum::MENU) {
            bool startGame = false;
            while (!window.shouldClose() && !startGame) {
				glViewport(0, 0, window.getWidth(), window.getHeight()); // reset viewport to ensure fullscreen
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

        PhysicsSystem* physicsSystem = new PhysicsSystem(gState, models);

        audio.init(physicsSystem, &camera);

        // Static scene data
        sceneModels.push_back(groundPlaneModel);

        physicsSystem->updateTransforms(gState.dynamicEntities);
        
        std::cout << "---------- resetting scoreboard" << std::endl;
        // reset scoreboard
		gState.initializeScores(numberOfPlayers, numberOfAiCars);
		uiManager.addScoreText(gState);


        // Main Loop
        timer.reset();
        timer.advance();
        while (!window.shouldClose() && gameState == GameStateEnum::PLAYING) {
            window.clear();
            timer.tick();
            input.poll();
            controller1.Update();
            audio.update();

            // Update physics
            while (timer.getAccumultor() > 5 && timer.getAccumultor() >= timer.dt) {
                physicsSystem->stepPhysics(timer.dt, command, controllerCommand);
                physicsSystem->updatePhysics(timer.dt);
                timer.advance();
            }
            
            // update dynamic UI text
            uiManager.updateUIText(timer, roundDuration, gState);

            // render everything except minimap
            renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
        
            // render minimap
			glDisable(GL_DEPTH_TEST);
			renderer.renderMinimap(minimapShader, minimapCamera);
			glEnable(GL_DEPTH_TEST);

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
            //sceneModels.clear();

            gState.reset();
            audio.stopMusic(); 
            timer.reset();
            delete physicsSystem;
            //delete audio;

        }

    }
    return 0;
}