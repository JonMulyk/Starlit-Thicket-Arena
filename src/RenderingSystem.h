#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "boilerplate/Shader.h"
#include "boilerplate/Texture.h"
#include "boilerplate/Camera.h"
#include "Text.h"
#include "Entity.h"

#include <string>
#include <map>
#include <vector>

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


		std::map<char, Character> charactersArial;

		void initializeGLFW();
		void initializeShaders(const std::string& vertexPath, const std::string& fragPath);
		void initializeTextRenderer();
		void initializeRenderData();


		// Camera stuff
		Camera camera;
		float lastX = static_cast<float>(windowWidth) / 2.0f;
		float lastY = static_cast<float>(windowHeight) / 2.0f;
		bool firstMouse = true;

	public:
		RenderingSystem(const int width, const int height, const std::string vertexPath, const std::string fragPath);
		virtual ~RenderingSystem();
		Shader shader;
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
		void updateRenderer(std::vector<Entity> entityList);

		// Callbacks
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};