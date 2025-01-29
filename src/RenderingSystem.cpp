#include "RenderingSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>


RenderingSystem::RenderingSystem(const int width, const int height, const std::string vertexPath, const std::string fragPath)
	: windowWidth(width), windowHeight(height)
{
	initializeGLFW();
	initializeShaders(vertexPath, fragPath);
	initializeTextRenderer();
	initializeRenderData();
}


RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::initializeGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	this->setWindow(windowWidth, windowHeight);

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

}


void RenderingSystem::initializeShaders(const std::string& vertexPath, const std::string& fragPath)
{
	// Initialize Shader
	Shader shader(vertexPath.c_str(), fragPath.c_str());
	ourShader = shader;
}

void RenderingSystem::initializeTextRenderer()
{
	// Initialize Text Renderer
	Shader textShaderTmp("./assets/shaders/textShader.vert", "./assets/shaders/textShader.frag");
	textShader = textShaderTmp;
	charactersArial = initFont("./assets/fonts/Arial.ttf");
	initTextVAO(&textVAO, &textVBO);

	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(1440), 0.0f, static_cast<float>(1440));
	textShader.use();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));
}

void RenderingSystem::initializeRenderData()
{
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	// Texture loading
	texture1 = generateTexture("./assets/textures/container.jpg", true);
	texture2 = generateTexture("./assets/textures/wall.jpg", true);
	
	// Render Triangle
	ourShader.use();
	
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	
	this->VAO = initTextureVAO(vertices, sizeof(vertices));


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

void RenderingSystem::updateRenderer()
{
	processInput();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	ourShader.use();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glBindVertexArray(getVAO());
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	// Render Text
	textShader.use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderText(textShader, textVAO, textVBO, "hello!", 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f), charactersArial);


	glfwSwapBuffers(getWindow());
	glfwPollEvents();
}
