#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm.hpp>
#include <exception> // for std::exception

class Windowing {
private:
    int m_width;
    int m_height;
    GLFWwindow* m_window;

public:
    // Constructor and destructor
    Windowing(int width, int height, std::string name = "Starlit Thicket Arena");
    ~Windowing();

    // Clear the window
    void clear(glm::vec4 c = glm::vec4(.2f, .1f, .3f, 1));

    void swapBuffer();

    // Returns whether the window should close.
    bool shouldClose();

    // Getters
    int getWidth() const;
    int getHeight() const;

    // Allows the Windowing object to be used where a GLFWwindow* is expected.
    operator GLFWwindow* () const;

    // Callback for when the framebuffer size changes.
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);

};
