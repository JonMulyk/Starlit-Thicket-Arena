#include "Windowing.h"
#include <exception>

Windowing::Windowing(int width, int height, std::string name) {
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

    glViewport(0, 0, width, height);

    // Set frame buffer size callback
    glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Windowing::~Windowing() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
}

bool Windowing::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

void Windowing::swapBuffer() {
    glfwSwapBuffers(m_window);
}

int Windowing::getHeight() const {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return height;
}

int Windowing::getWidth() const {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return width;
}

void Windowing::clear(glm::vec4 c) {
    // color for window
    glClearColor(c.r, c.g, c.b, c.a);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Windowing::operator GLFWwindow* () const { return m_window; }

void Windowing::frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
