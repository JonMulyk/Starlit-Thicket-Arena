#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "RenderingSystem.h"
#include "Text.h"


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


int main()
{
	// Time and Frame Vars
	double t = 0.0;
	const double dt = 1.0 / 60.0;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;
	
	// System Init
	RenderingSystem* renderer = new RenderingSystem(800, 600, "./assets/shaders/VertShader.vert", "./assets/shaders/FragShader.frag");
	glfwSetFramebufferSizeCallback(renderer->getWindow(), framebuffer_size_callback);
	//Shader shader();

	PhysicsSystem* physicsSystem = new PhysicsSystem();
	
	// Simple box example
	std::vector<Entity> entityList;
	unsigned int reserveNum = 465;
	entityList.reserve(reserveNum);
	for (int i = 0; i < reserveNum; i++)
	{
		entityList.emplace_back();
		entityList.back().name = "box";
		entityList.back().transform = physicsSystem->transformList[i];
		entityList.back().model = NULL;
	}

	// Main Game Loop
	while (!glfwWindowShouldClose(renderer->getWindow()))
	{
		// New time trackers
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		// Physics System Loop
		while (accumulator >= dt)
		{
			physicsSystem->updatePhysics(dt);
			accumulator -= dt;
			t += dt;
		}

		physx::PxVec3 objPos = physicsSystem->getPos(50);
		std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
		std::cout << entityList[50].transform->pos.y << std::endl;

		renderer->updateRenderer();
	}


	glfwTerminate();

	return 0;
}