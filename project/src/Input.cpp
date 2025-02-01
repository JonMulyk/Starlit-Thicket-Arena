#include "Input.h"
#include <iostream>

Input::Input(Windowing& window)
    : mouse_pressed(false), r_window(window)
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
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    // Debug output for key events
    std::cout << "key: " << key
        << "\tscancode: " << scancode
        << "\taction: " << action
        << "\tmods: " << mods << std::endl;
}

void Input::cursorPosCallback(GLFWwindow* window, double x_position, double y_position) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    // Debug output for cursor position events
    std::cout << "x-pos: " << x_position
        << "\ty-pos: " << y_position << std::endl;
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    // Debug output for mouse button events
    std::cout << "button: " << button
        << "\taction: " << action
        << "\tmods: " << mods << std::endl;
}

void Input::scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    Input* p_input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    // Debug output for scroll events
    std::cout << "x-offset: " << x_offset
        << "\ty-offset: " << y_offset << std::endl;
}
