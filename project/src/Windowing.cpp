#include "Windowing.h"
#include <exception>

Windowing::Windowing(int width, int height, std::string name)
    : m_width(width), m_height(height)
{
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if (m_window == nullptr) {
        glfwTerminate();
        // TODO: Create custom exception
        throw std::exception("Failed to create GLFW window\n");
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // TODO: Create custom exception
        throw std::exception("Failed to initialize GLAD\n");
    }

    glViewport(0, 0, m_width, m_height);

    // Set frame buffer size callback
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
}

Windowing::~Windowing() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
}

bool Windowing::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

Windowing::operator GLFWwindow* () const {
    return m_window;
}

void Windowing::frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    Windowing* p_window = static_cast<Windowing*>(glfwGetWindowUserPointer(window));
    if (p_window) {
        p_window->m_width = width;
        p_window->m_height = height;
    }
    glViewport(0, 0, width, height);
}
