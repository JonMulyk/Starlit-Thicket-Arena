#define TINYOBJLOADER_IMPLEMENTATION
#include "RenderingSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "tiny_obj_loader.h"

bool loadOBJ(const std::string& path, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texcoords)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
	{
		std::cerr << "Failed to load OBJ: " << warn + err << std::endl;
		return false;
	}

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			// Ensure vertex index is within bounds
			if (index.vertex_index >= 0 && index.vertex_index < attrib.vertices.size() / 3)
			{
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
			}

			// Check and add normals if present
			if (index.normal_index >= 0 && index.normal_index < attrib.normals.size() / 3)
			{
				normals.push_back(attrib.normals[3 * index.normal_index + 0]);
				normals.push_back(attrib.normals[3 * index.normal_index + 1]);
				normals.push_back(attrib.normals[3 * index.normal_index + 2]);
			}

			// Check and add texture coordinates if present
			if (index.texcoord_index >= 0 && index.texcoord_index < attrib.texcoords.size() / 2)
			{
				texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
				texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
			}
		}
	}

	return true;
}


RenderingSystem::RenderingSystem(const int width, const int height, const std::string vertexPath, const std::string fragPath)
	: windowWidth(width), windowHeight(height)
{
	initializeGLFW();
	initializeShaders(vertexPath, fragPath);
	initializeTextRenderer();
	initializeRenderData();


	glfwSetWindowUserPointer(window, this); // get a reference to our rendering system
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
}


RenderingSystem::~RenderingSystem(){}

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
	Shader tempShader(vertexPath.c_str(), fragPath.c_str());
	shader = tempShader;
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
	// vertices for a cube
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// Texture loading
	texture1 = generateTexture("./assets/textures/container.jpg", true);
	texture2 = generateTexture("./assets/textures/wall.jpg", true);
		
	// Render Triangle
	shader.use();

	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	this->VAO = initTextureVAO(vertices, sizeof(vertices));



	/*
	std::vector<float> vertices, normals, texcoords;
	if (!loadOBJ("./assets/models/GTree.obj", vertices, normals, texcoords))
	{
		throw std::runtime_error("Failed to load OBJ file.");
	}
	*/

	/*
	for (size_t i = 0; i < vertices.size(); i += 3) {
		vertices[i + 1] -= 1.0f;
	}

	float scale = 0.5f;
	for (size_t i = 0; i < vertices.size(); i += 3) {
		vertices[i] *= scale;     // x
		vertices[i + 1] *= scale; // y
		vertices[i + 2] *= scale; // z
	}

	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->setVAO(vao);
	this->setVBO(vbo);
	*/

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
	this->window = glfwCreateWindow(width, height, "Starlit Thicket Arena", NULL, NULL);
}

void RenderingSystem::processInput()
{
	if (glfwGetKey(this->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->getWindow(), true);
	}

	// Camera code (wasd)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(camera.FORWARD, 0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(camera.BACKWARD, 0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(camera.RIGHT, 0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(camera.LEFT, 0.01f);
	}
}

void RenderingSystem::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Retrieve the instance of RenderingSystem from the window user pointer
	RenderingSystem* renderingSystem = static_cast<RenderingSystem*>(glfwGetWindowUserPointer(window));

	if (renderingSystem) {
		renderingSystem->camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

void RenderingSystem::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

	// Retrieve the instance of RenderingSystem from the window user pointer
	RenderingSystem* renderingSystem = static_cast<RenderingSystem*>(glfwGetWindowUserPointer(window));

	if (renderingSystem) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (renderingSystem->firstMouse)
		{
			renderingSystem->lastX = xpos;
			renderingSystem->lastY = ypos;
			renderingSystem->firstMouse = false;
		}

		float xoffset = xpos - renderingSystem->lastX;
		float yoffset = renderingSystem->lastY - ypos; // reversed since y-coordinates go from bottom to top

		renderingSystem->lastX = xpos;
		renderingSystem->lastY = ypos;

		renderingSystem->camera.ProcessMouseMovement(xoffset, yoffset);
	}

}

void RenderingSystem::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	RenderingSystem* renderingSystem = static_cast<RenderingSystem*>(glfwGetWindowUserPointer(window));
	renderingSystem->windowWidth = width;
	renderingSystem->windowHeight = height;
	glViewport(0, 0, width, height);
}


void RenderingSystem::updateRenderer(std::vector<Entity> entityList)
{
	processInput();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	shader.use();

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(this->VAO);
	
	glm::mat4 projection = glm::perspective(
		glm::radians(this->camera.Zoom),
		(static_cast<float>(this->windowWidth) /  static_cast<float>(this->windowHeight)), 
		0.1f, 
		100.0f
	);
	shader.setMat4("projection", projection);

	// camera / view transformation
	glm::mat4 view = this->camera.GetViewMatrix();
	shader.setMat4("view", view);

	// Render Physics
	for (int i = 0; i < entityList.size(); i++)
	{
		glm::vec3 pos = entityList.at(i).transform->pos;
		glm::quat rot = entityList.at(i).transform->rot;

		glm::mat4 model = glm::mat4(1.0f);				// initial matrix
		model = glm::translate(model, pos);				// translate
		model = model * glm::mat4_cast(rot);			// rotate
		model = glm::scale(model, glm::vec3(1.0f));		// scale
		shader.setMat4("model", model);					// pass to shader

		// GL calls
		glDrawArrays(GL_TRIANGLES, 0, 36);				// Draw for each 
		glActiveTexture(GL_TEXTURE0);					// texture 1
		glBindTexture(GL_TEXTURE_2D, texture1);			
		glActiveTexture(GL_TEXTURE1);					// texture 2
		glBindTexture(GL_TEXTURE_2D, texture2);			
		glBindVertexArray(this->VAO);					// VAO for each
	}

	// Render Text
	textShader.use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderText(textShader, textVAO, textVBO, "hello!", 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f), charactersArial);


	glfwPollEvents();
	glfwSwapBuffers(getWindow());
}
