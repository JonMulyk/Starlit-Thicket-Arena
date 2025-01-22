#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PxPhysicsAPI.h"
#include "PhysicsSystem.h"
#include "Entity.h"

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

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Starlit Thicket Arena", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}