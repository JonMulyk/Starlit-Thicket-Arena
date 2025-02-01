#pragma once

#include "Windowing.h"
#include <GLFW/glfw3.h>

class Input {
private:
    bool mouse_pressed;
    Windowing& r_window;

public:
    // Constructor
    Input(Windowing& window);

    // Polls for and processes events
    void poll();

    // Callback functions for various input events
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x_position, double y_position);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);
};
