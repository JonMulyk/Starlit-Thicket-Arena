#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "boilerplate/Shader.h"


class RenderingSystem
{
	private:
		int windowWidth;
		int windowHeight;

		GLFWwindow* window;
		
		unsigned int VAO;
		unsigned int VBO;

	public:
		RenderingSystem(const int width, const int height);
		//Shader shader;
	
		unsigned int getVAO();
		void setVAO(const unsigned int value);
		unsigned int getVBO();
		void setVBO(const unsigned int value);

		//Shader getShader();

		GLFWwindow* getWindow() const;
		void setWindow(const int width, const int height);


		void processInput();
};