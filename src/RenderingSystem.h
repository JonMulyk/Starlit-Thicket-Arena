#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "boilerplate/Shader.h"
#include "boilerplate/Texture.h"
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

		std::map<char, Character> charactersArial;

		void initializeGLFW();
		void initializeShaders(const std::string& vertexPath, const std::string& fragPath);
		void initializeTextRenderer();
		void initializeRenderData();

	public:
		RenderingSystem(const int width, const int height, const std::string vertexPath, const std::string fragPath);
		virtual ~RenderingSystem();
		Shader ourShader;
		Shader textShader;
	
		unsigned int getVAO();
		void setVAO(const unsigned int value);
		unsigned int getVBO();
		void setVBO(const unsigned int value);

		GLFWwindow* getWindow() const;
		void setWindow(const int width, const int height);


		void processInput();
		void updateRenderer();
};