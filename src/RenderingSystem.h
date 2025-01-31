#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "boilerplate/Shader.h"
#include "boilerplate/Texture.h"
#include "boilerplate/Camera.h"
#include "Text.h"

#include <string>
#include <map>

class RenderingSystem
{
	private:
		int windowWidth;
		int windowHeight;

		GLFWwindow* window;
		
		unsigned int VAO;
		unsigned int VBO;
		unsigned int textVAO;
		unsigned int textVBO;

		glm::vec3 cubePositions[10] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};


		std::map<char, Character> charactersArial;

		void initializeGLFW();
		void initializeShaders(const std::string& vertexPath, const std::string& fragPath);
		void initializeTextRenderer();
		void initializeRenderData();


		// Camera stuff
		Camera camera;
		float lastX = windowWidth / 2.0f;
		float lastY = windowHeight / 2.0f;
		bool firstMouse = true;

	public:
		RenderingSystem(const int width, const int height, const std::string vertexPath, const std::string fragPath);
		virtual ~RenderingSystem();
		Shader ourShader;
		Shader textShader;

		unsigned int texture1;
		unsigned int texture2;

		unsigned int getVAO();
		void setVAO(const unsigned int value);
		unsigned int getVBO();
		void setVBO(const unsigned int value);

		GLFWwindow* getWindow() const;
		void setWindow(const int width, const int height);


		void processInput();

		// Callbacks
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


		void updateRenderer();
};