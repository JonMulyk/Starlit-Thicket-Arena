#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "RenderingSystem.h"

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
	/* PHYSICS CODE COMMENTED OUT FOR NOW
	PhysicsSystem physicsSys;

	// std::cout << physicsSys.rigidDynamicList.size() << std::endl;

	// Simulate at 60fps

	std::vector<Entity> entityList;
	entityList.reserve(465);

	for (int i = 0; i < 465; i++)
	{
		entityList.emplace_back();
		entityList.back().name = "box";
		entityList.back().transform = physicsSys.transformList[i];
		entityList.back().model = NULL;
	}

	while (1)
	{
		physicsSys.gScene->simulate(1.0f / 60.0f);
		physicsSys.gScene->fetchResults(true);
		physicsSys.updateTransforms();

		physx::PxVec3 objPos = physicsSys.getPos(50);
		std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
		std::cout << entityList[50].transform->pos.y << std::endl;
	}
	*/

	RenderingSystem* renderer = new RenderingSystem(800, 600);
	glfwSetFramebufferSizeCallback(renderer->getWindow(), framebuffer_size_callback);
	

	while (!glfwWindowShouldClose(renderer->getWindow()))
	{
		glClearColor(1.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		glfwSwapBuffers(renderer->getWindow());
	}

	glfwTerminate();

	return 0;
}