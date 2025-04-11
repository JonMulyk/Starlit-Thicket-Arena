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
#include "splitScreenSelect.h"
#include "TransparentBoxRenderer.h"
#include <PauseMenu.h>



int main() {
    GameState gState;
    InitManager::initGLFW();
    Command command;
    Command controllerCommand1;
    Command controllerCommand2;
    Command controllerCommand3;
    Command controllerCommand4;
    Command controllerCommand;
    TimeSeconds timer;
    DynamicCamera camera(gState, timer);
    DynamicCamera camera2(gState, timer);
    DynamicCamera camera3(gState, timer);
    DynamicCamera camera4(gState, timer);

    //Windowing window(1200, 1000, "", false);
    Windowing window(1920, 1080);
    Input input(window, camera, timer, command);
    Controller controller1(1, camera, controllerCommand1);
	Controller controller2(2, camera2, controllerCommand2);
	Controller controller3(3, camera3, controllerCommand3);
	Controller controller4(4, camera4, controllerCommand4);
    if (!controller1.isConnected()) {
        std::cout << "Controller one not connected" << std::endl; 
        controllerCommand1.brake = 0.0f; controllerCommand1.throttle = 0.0f; controllerCommand1.steer = 0.0f;
    }
	if (!controller2.isConnected()) {
		std::cout << "Controller two not connected" << std::endl;
		controllerCommand2.brake = 0.0f; controllerCommand2.throttle = 0.0f; controllerCommand2.steer = 0.0f;
	}
	if (!controller3.isConnected()) {
		std::cout << "Controller three not connected" << std::endl;
		controllerCommand3.brake = 0.0f; controllerCommand3.throttle = 0.0f; controllerCommand3.steer = 0.0f;
	}
	if (!controller4.isConnected()) {
		std::cout << "Controller four not connected" << std::endl;
		controllerCommand4.brake = 0.0f; controllerCommand4.throttle = 0.0f; controllerCommand4.steer = 0.0f;
	}

    Shader shader("basicShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Shader lightingShader("lightingShader", "project/assets/shaders/lightingShader.vert", "project/assets/shaders/lightingShader.frag");
    TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf", window.getWidth(), window.getHeight());
    Texture container("project/assets/textures/container.jpg", true);
    Texture gold("project/assets/textures/gold.jpg", true);
    Texture neon("project/assets/textures/neon.jpg", true);
    Texture fire("project/assets/textures/fire.jpg", true);
    Texture grass("project/assets/textures/green-grass.jpg", true);
    Texture blueGrass("project/assets/textures/696.jpg", true);
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
    TransparentBoxRenderer boxRenderer;

    Model groundPlaneModel(sceneShader, neon, "project/assets/models/reallySquareArena.obj");
    Model groundPlaneModel2(sceneShader, grass, "project/assets/models/reallySquareArena.obj");
    Model groundPlaneModel3(sceneShader, blueGrass, "project/assets/models/reallySquareArena.obj");
    UIManager uiManager(window.getWidth(), window.getHeight());
    int selectedLevel = -1;
    RenderingSystem renderer(shader, camera, window, arial, gState);
    RenderingSystem renderer2(shader, camera2, window, arial, gState);
    RenderingSystem renderer3(shader, camera3, window, arial, gState);
    RenderingSystem renderer4(shader, camera4, window, arial, gState);
    const double roundDuration = 30;

    bool isAudioInitialized = false;

    AudioSystem audio;

    MainMenu menu(window, arial, controller1);
    LevelSelectMenu levelSelectMenu(window, arial, controller1, gState);
    splitScreenSelect splitScreenSelectMenu(window, arial, controller1);
    PauseScreen pause(window, arial, controller1, audio);

    std::vector<Model> models = { Gtrail, Btrail, Rtrail, Ytrail, secondCar, cube };

    // Minimap 
    Shader minimapShader("minimapShader", "project/assets/shaders/minimapShader.vert", "project/assets/shaders/minimapShader.frag");
    StaticCamera minimapCamera(timer, glm::vec3(0.0f, 250.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Number of players playing for scoreboard 
    //uint16_t numberOfPlayers = 1;
    //uint16_t numberOfAiCars = 3;

    // Main Loop
    //timer.advance();
    while (!window.shouldClose()) {
        if (gameState == GameStateEnum::MENU) {
            bool startGame = false;
            // Display main menu and level select until a valid level is chosen.
            while (!window.shouldClose() && !startGame) {
                glViewport(0, 0, window.getWidth(), window.getHeight()); // reset viewport to ensure fullscreen
                menu.displayMenu();
                if (window.shouldClose()) break;
                selectedLevel = levelSelectMenu.displayMenuLevel();
                if (selectedLevel != -1)
                    startGame = true;
            }
            if (!startGame || window.shouldClose())
                return 0;

            // Now display the split screen selection menu.
            splitScreenSelect splitMenu(window, arial, controller1);
            int splitChoice = splitMenu.displayMenuLevel();
            if (splitChoice == -1) { continue; }
            else if (splitChoice == 1) { gState.splitScreenEnabled = false; gState.splitScreenEnabled4 = false; }
            else if (splitChoice == 2) { gState.splitScreenEnabled = true; gState.splitScreenEnabled4 = false; }
            else if (splitChoice == 3) { gState.splitScreenEnabled = false; gState.splitScreenEnabled4 = true; }

            // Proceed to start the game.
            gameState = GameStateEnum::PLAYING;
        }

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

        // Game setup
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!controller1.isConnected()) {
            std::cout << "Controller one not connected" << std::endl;
            controllerCommand1.brake = 0.0f; controllerCommand1.throttle = 0.0f; controllerCommand1.steer = 0.0f;
        }
        if (!controller2.isConnected()) {
            std::cout << "Controller two not connected" << std::endl;
            controllerCommand2.brake = 0.0f; controllerCommand2.throttle = 0.0f; controllerCommand2.steer = 0.0f;
        }
        if (!controller3.isConnected()) {
            std::cout << "Controller three not connected" << std::endl;
            controllerCommand3.brake = 0.0f; controllerCommand3.throttle = 0.0f; controllerCommand3.steer = 0.0f;
        }
        if (!controller4.isConnected()) {
            std::cout << "Controller four not connected" << std::endl;
            controllerCommand4.brake = 0.0f; controllerCommand4.throttle = 0.0f; controllerCommand4.steer = 0.0f;
        }

        PhysicsSystem* physicsSystem = new PhysicsSystem(gState, models);
        camera.setFollowTarget(physicsSystem->getTransformAt("playerCar"));
        camera2.setFollowTarget(physicsSystem->getTransformAt("aiCar1"));
        camera3.setFollowTarget(physicsSystem->getTransformAt("aiCar2"));
        camera4.setFollowTarget(physicsSystem->getTransformAt("aiCar3"));

        audio.init(physicsSystem, &camera);

        // Static scene data
        //sceneModels.push_back(groundPlaneModel);

        physicsSystem->updateTransforms(gState.dynamicEntities);

        // reset scoreboard
        //gState.initializeScores(numberOfPlayers, numberOfAiCars);
        if (gState.splitScreenEnabled) gState.initializeScores(2, 2);
		else if (gState.splitScreenEnabled4) gState.initializeScores(4, 0);
		else gState.initializeScores(1, 3);
        uiManager.addScoreText(gState);


        // Main Loop
        timer.reset();
        timer.advance();
        while (!window.shouldClose() && gameState == GameStateEnum::PLAYING) {
            window.clear();
            timer.tick();
            input.poll();
            audio.update();

            controller1.Update();
            if (gState.splitScreenEnabled || gState.splitScreenEnabled4) {
                controller2.Update();
            }
            if (gState.splitScreenEnabled4) {
                controller3.Update();
                controller4.Update();
            }

            // Vector of controller Commands
            std::vector<Command*> controllerCommands;
            controllerCommands.push_back(&controllerCommand1);
            controllerCommands.push_back(&controllerCommand2);
            controllerCommands.push_back(&controllerCommand3);
            controllerCommands.push_back(&controllerCommand4);

            if (physicsSystem->deadCars[0] == 0) camera.updateZoom(physicsSystem->getCarSpeed(0));
            //camera.updateYawWithDelay(glm::degrees(atan2(gState.playerVehicle.curDir.z, gState.playerVehicle.curDir.x)), timer.dt);
            if (physicsSystem->deadCars[1] == 0) camera2.updateZoom(physicsSystem->getCarSpeed(1));
			//camera2.updateYawWithDelay(glm::degrees(atan2(gState.playerVehicle2.curDir.z, gState.playerVehicle2.curDir.x)), timer.dt);
            if (physicsSystem->deadCars[2] == 0) camera3.updateZoom(physicsSystem->getCarSpeed(2));
			//camera3.updateYawWithDelay(glm::degrees(atan2(gState.playerVehicle3.curDir.z, gState.playerVehicle3.curDir.x)), timer.dt);
            if (physicsSystem->deadCars[3] == 0) camera4.updateZoom(physicsSystem->getCarSpeed(3));
			//camera4.updateYawWithDelay(glm::degrees(atan2(gState.playerVehicle4.curDir.z, gState.playerVehicle4.curDir.x)), timer.dt);

            physicsSystem->update(timer.getFrameTime());

            if (physicsSystem->playerDied || physicsSystem->deadCars[0] == 1) {
                audio.stopCarSounds(); 
                audio.stopAISounds();
            }
			else if (physicsSystem->player2Died) audio.stopAISounds(physicsSystem->getCarPos("aiCar1"));
			else if (physicsSystem->player3Died) audio.stopAISounds(physicsSystem->getCarPos("aiCar2"));
			else if (physicsSystem->player4Died) audio.stopAISounds(physicsSystem->getCarPos("aiCar3"));
            else {
                audio.startCarSounds();  
                audio.update();
            }

            physicsSystem->update(timer.getFrameTime());

            //pause
            if (input.isKeyReleased(GLFW_KEY_P) || controller1.isButtonJustReleased(XINPUT_GAMEPAD_START)) {
                gameState = GameStateEnum::PAUSE;
                timer.stop();
                audio.pauseMusic();

            }

            while (gameState == GameStateEnum::PAUSE) {
                input.poll();
                controller1.Update();
                window.clear();
                bool resumeGame = pause.displayPauseScreen();

                if (resumeGame) {
                    gameState = GameStateEnum::PLAYING;
                    timer.resume();
                    audio.resumePauseSounds();
                }
                else {
                    //back to main menu
                    gameState = GameStateEnum::RESET;
                    //break; //might need this...
                }
            }

            // Update physics
            while (timer.getAccumultor() > 5 && timer.getAccumultor() >= timer.dt) {
                physicsSystem->stepPhysics(timer.dt, command, controllerCommands);
                physicsSystem->updatePhysics(timer.dt);
                timer.advance();
            }

            if (gState.splitScreenEnabled) {
                //std::cout << "here\n";
                if (physicsSystem->deadCars[0] == 0) camera.setFollowTarget(physicsSystem->getTransformAt("playerCar"));
                if (physicsSystem->deadCars[1] == 0) camera2.setFollowTarget(physicsSystem->getTransformAt("aiCar1"));
                // Left half
                glViewport(0, window.getHeight() / 2, window.getWidth(), window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 0);
                renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);  // Using camera1 (a Camera instance)
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 0);
                glEnable(GL_DEPTH_TEST);

                // Right half
                //print camera2 position
                glViewport(0, 0, window.getWidth(), window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 1);
                renderer2.updateRenderer(sceneModels, uiManager.getUIText(), skybox);  // Using camera2 (a Camera instance)
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 1);
                glEnable(GL_DEPTH_TEST);
            }
            else if (gState.splitScreenEnabled4) {
                if (physicsSystem->deadCars[0] == 0) camera.setFollowTarget(physicsSystem->getTransformAt("playerCar"));
                if (physicsSystem->deadCars[1] == 0) camera2.setFollowTarget(physicsSystem->getTransformAt("aiCar1"));
                if (physicsSystem->deadCars[2] == 0) camera3.setFollowTarget(physicsSystem->getTransformAt("aiCar2"));
                if (physicsSystem->deadCars[3] == 0) camera4.setFollowTarget(physicsSystem->getTransformAt("aiCar3"));
                // split the screen into 4 separate quadrants
                // top left
                glViewport(0, window.getHeight() / 2, window.getWidth() / 2, window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 0);
                renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 0);
                glEnable(GL_DEPTH_TEST);

                // top right
                glViewport(window.getWidth() / 2, window.getHeight() / 2, window.getWidth() / 2, window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 1);
                renderer2.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 1);
                glEnable(GL_DEPTH_TEST);

                // bottom left
                glViewport(0, 0, window.getWidth() / 2, window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 2);
                renderer3.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 2);
                glEnable(GL_DEPTH_TEST);

                // bottom right
                glViewport(window.getWidth() / 2, 0, window.getWidth() / 2, window.getHeight() / 2);
                uiManager.updateUIText(timer, roundDuration, gState, 3);
                renderer4.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 3);
                glEnable(GL_DEPTH_TEST);
            }
            else {
                // update dynamic UI text
                uiManager.updateUIText(timer, roundDuration, gState, 0);

                // render everything except minimap
                renderer.updateRenderer(sceneModels, uiManager.getUIText(), skybox);
                // render minimap
                glDisable(GL_DEPTH_TEST);
                renderer.renderMinimap(minimapShader, minimapCamera, 0);
                glEnable(GL_DEPTH_TEST);
            }

            glfwSwapBuffers(window);

            // Check for round end
            if (timer.getElapsedTime() >= roundDuration) {
                gameState = GameStateEnum::RESET;
            }
        }

        //reset
        if (gameState == GameStateEnum::RESET) {
            //pauseResult = PauseResult::RESUME;
            //command.fuel = 75; //should this be a thing to reset?
            gameState = GameStateEnum::MENU;
            gState.dynamicEntities.clear();
            gState.staticEntities.clear();
            sceneModels.clear();

            gState.reset();
            command.reset();
            audio.stopMusic(); 
            timer.reset();
            delete physicsSystem;
            //delete audio;
        }

    }
    return 0;
}