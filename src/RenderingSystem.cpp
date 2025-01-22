#include "RenderingSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>


RenderingSystem::RenderingSystem(const int width, const int height)
	: windowWidth(width), windowHeight(height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	this->setWindow(width, height);

	if (this->getWindow() == NULL)
	{
		glfwTerminate();
		throw std::exception("Failed to create GLFW window\n"); //FUTURE -> create custom exception here
	}

	glfwMakeContextCurrent(this->getWindow());


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::exception("Failed to initialize GLAD\n"); //FUTURE -> create custom exception here
	}

	glViewport(0, 0, windowWidth, windowHeight);

	// Shader
//	Shader shader("../assets/shaders/VertShader.vert", "../assets/shaders/FragShader.frag");
	//this->shader("", "");

	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}


unsigned int RenderingSystem::getVAO()
{
	return this->VAO;
}

void RenderingSystem::setVAO(const unsigned int vaoValue)
{
	this->VAO = vaoValue;
}

unsigned int RenderingSystem::getVBO()
{
	return this->VBO;
}

void RenderingSystem::setVBO(const unsigned int vboValue)
{
	this->VBO = vboValue;
}

GLFWwindow* RenderingSystem::getWindow() const
{
	return window;
}

void RenderingSystem::setWindow(const int width, const int height)
{
	this->window = glfwCreateWindow(800, 600, "Starlit Thicket Arena", NULL, NULL);
}

void RenderingSystem::processInput()
{
	if (glfwGetKey(this->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->getWindow(), true);
	}
}

