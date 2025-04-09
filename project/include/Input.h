#pragma once

#include "PhysicsSystem.h"
#include "Camera.h"
#include "Windowing.h"
#include "TimeSeconds.h"
#include <GLFW/glfw3.h>

class Input {
private:

    bool firstMouse = true;
    double prevPosX = 0;
    double prevPosY = 0;

    TimeSeconds& r_timer;
    Camera& r_camera;
    Windowing& r_window;
    Command& r_command;
public:

    // Constructor
    Input(Windowing& window, Camera& camera, TimeSeconds& timer, Command& command);


    void processKeyboard();

    // Polls for and processes events
    void poll();

    // Callback functions for various input events
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x_position, double y_position);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);
    bool isKeyReleased(int key);
};
