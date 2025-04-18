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
#include "TransparentBoxRenderer.h"
#include <PauseMenu.h>

int main() {
    InitManager::initGLFW();
    GameState gState;       // tracks game updates and state
    TimeSeconds timer;      // tracks the game time

    // Cameras
    DynamicCamera camera(gState, timer);
    StaticCamera minimapCamera(
        timer,
        glm::vec3(0.0f, 250.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    Windowing window(1920, 1080);   // full screen window using glfw

    // Inputs
    Command command;
    Input input(window, camera, timer, command);
    Controller controller1(1, camera, command);
    if (!controller1.isConnected()) {
        std::cout << "Controller one not connected" << std::endl;
    }

    // True Text Fonts
    TTF arial(
        "project/assets/shaders/textShader.vert",
        "project/assets/shaders/textShader.frag",
        "project/assets/fonts/Arial.ttf",
        window.getWidth(), window.getHeight()
    );
    
    // Shaders
    Shader shader("basicShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Shader lightingShader("lightingShader", "project/assets/shaders/lightingShader.vert", "project/assets/shaders/lightingShader.frag");
    Shader skyboxShader("project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
    Shader sceneShader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    Shader minimapShader("minimapShader", "project/assets/shaders/minimapShader.vert", "project/assets/shaders/minimapShader.frag");

    // Textures
    Texture container("project/assets/textures/container.jpg", true);
    Texture gold("project/assets/textures/gold.jpg", true);
    Texture neon("project/assets/textures/neon.jpg", true);
    Texture fire("project/assets/textures/fire.jpg", true);
    Texture grass("project/assets/textures/green-grass.jpg", true);
    Texture blueGrass("project/assets/textures/696.jpg", true);

    // Models - manual entries
    std::vector<float> verts, coord, norms;
    InitManager::getCube(verts, coord);
    Model cube(lightingShader, container, verts, verts, coord);

    InitManager::getGround(verts, norms, coord);
    Model groundPlaneModel(sceneShader, neon, verts, norms, coord);
    Model groundPlaneModel2(sceneShader, grass, verts, norms, coord);
    Model groundPlaneModel3(sceneShader, blueGrass, verts, norms, coord);

    // Models - obj
    Model redBrick(lightingShader, gold, "project/assets/models/box.obj");
    Model Gtrail(lightingShader, "project/assets/models/Gtree/GTree.obj");
    Model Btrail(lightingShader, "project/assets/models/Btree/BTree.obj");
    Model Rtrail(lightingShader, "project/assets/models/Rtree/RTree.obj");
    Model Ytrail(lightingShader, "project/assets/models/Ytree/YTree.obj");
    Model tireModel(lightingShader, "project/assets/models/tire1/tire1.obj");
    Model secondCar(shader, "project/assets/models/bike/Futuristic_Car_2.1_obj.obj");

    // Model Arrays
    std::vector<Model> sceneModels;
    std::vector<Model> models = { Gtrail, Btrail, Rtrail, Ytrail, secondCar, cube };

    // Skybox
    Skybox skybox("project/assets/textures/skybox/water/", skyboxShader);
    TransparentBoxRenderer boxRenderer;

    // Audio setup
    bool isAudioInitialized = false;
    AudioSystem audio;

    // UI
    UIManager uiManager(window.getWidth(), window.getHeight());
    MainMenu menu(window, arial, controller1);
    LevelSelectMenu levelSelectMenu(window, arial, controller1, gState);
    PauseScreen pause(window, arial, controller1, audio);

    // Rendering Pipeline
    RenderingSystem renderer(shader, camera, window, arial, gState);

    // Set game parameters
    int selectedLevel = -1;
    const double roundDuration = 200;
    GameStateEnum gameState = GameStateEnum::MENU;
    uint16_t numberOfPlayers = 1;
    uint16_t numberOfAiCars = 3;

    // Main Loop
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

        if (selectedLevel == 1) {
            sceneModels.push_back(groundPlaneModel);
        } else if (selectedLevel == 2)
        {
            sceneModels.push_back(groundPlaneModel2);
        } else if (selectedLevel == 3)
        {
            sceneModels.push_back(groundPlaneModel3);
        }

        // Game setup
        glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        PhysicsSystem* physicsSystem = new PhysicsSystem(gState, models);
        physicsSystem->updateTransforms(gState.dynamicEntities);
        audio.init(physicsSystem, &camera);

        // reset scoreboard
        gState.initializeScores(numberOfPlayers, numberOfAiCars);
        uiManager.addScoreText(gState);

        // -- Main Loop
        // Reset time
        timer.reset();
        timer.advance();
// ----------------------------------------------------------------------
        // Start loop
        while (!window.shouldClose() && gameState == GameStateEnum::PLAYING) {
            window.clear(); // clear window
            timer.tick(); // increment timer
            input.poll(); // check inputs
            controller1.Update(); // check controller

            // update camera
            camera.updateZoom(physicsSystem->getCarSpeed(0));
            camera.updateYawWithDelay(glm::degrees(atan2(gState.playerVehicle.curDir.z, gState.playerVehicle.curDir.x)), timer.dt);

            // update the audio
            if (physicsSystem->playerDied) {
                audio.stopCarSounds(); 
            }
            else {
                audio.startCarSounds();  
                audio.update();
            }

            // check for reinitialization
            if (physicsSystem->playerDied) {
                command.fuel = 1;
            }
            physicsSystem->update(timer.getFrameTime());

            // trigger pause
            if (input.isKeyReleased(GLFW_KEY_P) || controller1.isButtonJustReleased(XINPUT_GAMEPAD_START)) {
                gameState = GameStateEnum::PAUSE;
                timer.stop();
                audio.pauseMusic();
            }

            // pause state
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
                    gameState = GameStateEnum::RESET;
                }
            }

            // run game simulation
            while (timer.getAccumultor() > 5 && timer.getAccumultor() >= timer.dt) {
                physicsSystem->stepPhysics(timer.dt, command);
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
            //pauseResult = PauseResult::RESUME;
            command.fuel = 1;
            gameState = GameStateEnum::MENU;
            gState.dynamicEntities.clear();
            gState.staticEntities.clear();
            sceneModels.clear();

            gState.reset();
            command.reset();
            audio.stopMusic(); 
            timer.reset();
            delete physicsSystem;
            //TODO: free audio;
        }

    }
    return 0;
}