#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm.hpp>

#include "Windowing.h"

class MainMenu {
public:
    MainMenu(Windowing& window) : window(window) {}

    void displayMenu() {
        bool inMenu = true;
        while (inMenu && !window.shouldClose()) {
            window.clear();
            renderMenu();
            window.swapBuffer();
            glfwPollEvents();

            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
                inMenu = false; 
            }
            if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window.getGLFWwindow(), true);
            }
        }
    }

private:
    Windowing& window;

    void renderMenu() {
        std::cout << "\n=== MAIN MENU ===" << std::endl;
        std::cout << "Press ENTER to Start" << std::endl;
        std::cout << "Press ESC to Exit" << std::endl;
    }
};