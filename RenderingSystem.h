#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class RenderingSystem
{
	private:
		int windowWidth;
		int windowHeight;
		GLFWwindow* window;

	public:
		RenderingSystem(const int width, const int height);

		GLFWwindow* getWindow() const;
		void setWindow(const int width, const int height);

		void processInput();
};