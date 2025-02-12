#include <iostream>
#include "Input.h"

Input::Input(Windowing& window, Camera& camera, TimeSeconds& timer, Command& command)
    : r_window(window), r_camera(camera), r_timer(timer), r_command(command)
{
    // Set the user pointer to this instance so that callbacks can retrieve it.
    glfwSetWindowUserPointer(r_window, this);

    // Set up the callback functions for keyboard, mouse, and scroll events.
    glfwSetKeyCallback(r_window, keyCallback);
    glfwSetCursorPosCallback(r_window, cursorPosCallback);
    glfwSetMouseButtonCallback(r_window, mouseButtonCallback);
    glfwSetScrollCallback(r_window, scrollCallback);

    // TODO: Add controller support if needed.
}

void Input::poll() {
    glfwPollEvents();
    processKeyboard();
}

void Input::processKeyboard() {
    double deltaTime = r_timer.getFrameTime();

    // Car movement (wasd)
    if (glfwGetKey(r_window, GLFW_KEY_W) == GLFW_PRESS) {
        r_command.throttle = 1.0;
    } else {
        r_command.throttle = 0.0;
    }

    if (glfwGetKey(r_window, GLFW_KEY_S) == GLFW_PRESS) {
        r_command.brake = 1.0;
    } else {
        r_command.brake = 0.0;
    }

    r_command.steer = 0.0;
    if (glfwGetKey(r_window, GLFW_KEY_D) == GLFW_PRESS) {
        r_command.steer -= 1.0;
    }
    if (glfwGetKey(r_window, GLFW_KEY_A) == GLFW_PRESS) {
        r_command.steer += 1.0;
    }

    // Camera movement (arrows)
    if (glfwGetKey(r_window, GLFW_KEY_UP) == GLFW_PRESS) {
        r_camera.ProcessKeyboard(r_camera.FORWARD, deltaTime);
    }
    if (glfwGetKey(r_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        r_camera.ProcessKeyboard(r_camera.BACKWARD, deltaTime);
    }
    if (glfwGetKey(r_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        r_camera.ProcessKeyboard(r_camera.RIGHT, deltaTime);
    }
    if (glfwGetKey(r_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        r_camera.ProcessKeyboard(r_camera.LEFT, deltaTime);
    }
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case (GLFW_KEY_ESCAPE):
            glfwSetWindowShouldClose(window, true);
            break;
        default:
            break;
        }
    }

    /*
    // Debug output for key events
    std::cout << "key: " << key
        << "\tscancode: " << scancode
        << "\taction: " << action
        << "\tmods: " << mods << std::endl;
     */
}

void Input::cursorPosCallback(GLFWwindow* window, double x_position, double y_position) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (p_input == nullptr) {
        return;
    }

    if (p_input->firstMouse) {
        p_input->prevPosX = x_position;
        p_input->prevPosX = y_position;
        p_input->firstMouse = false;
    }

    float xoffset = x_position - p_input->prevPosX;
    float yoffset = p_input->prevPosY - y_position; // Y-coordinates go from bottom to top

    p_input->prevPosX = x_position;
    p_input->prevPosY = y_position;

    p_input->r_camera.ProcessMouseMovement(xoffset, yoffset);

    /*
    // Debug output for cursor position events
    std::cout << "x-pos: " << x_position
        << "\ty-pos: " << y_position << std::endl;
    */
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (p_input == nullptr) {
        return;
    }

    // Debug output for mouse button events
    std::cout << "button: " << button
        << "\taction: " << action
        << "\tmods: " << mods << std::endl;
}

void Input::scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (p_input == nullptr) {
        return;
    }

    p_input->r_camera.ProcessMouseScroll(y_offset);

    /*
    // Debug output for scroll events
    std::cout << "x-offset: " << x_offset
        << "\ty-offset: " << y_offset << std::endl;
    */
}
