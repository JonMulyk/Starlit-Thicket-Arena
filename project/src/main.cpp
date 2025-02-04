#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>

#include "TimeSeconds.h"
#include "InitManager.h"
#include "Windowing.h"
#include "Input.h"
#include "Shader.h"
#include "Texture.h"
#include "TTF.h"
#include "Model.h"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "Camera.h"

/*
m for members
c for constants / readonlys
p for pointer(and pp for pointer to pointer)
v for volatile
s for static
i for indexesand iterators
e for events
r for reference
*/

int main() {
	InitManager::initGLFW();

	TimeSeconds timer;
	Windowing window(1000, 800);
	Input input(window);
	Shader shader("project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
	TTF arial("project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
	Texture texture("project/assets/textures/container.jpg", true);
	Camera camera;

	PhysicsSystem* physicsSystem = new PhysicsSystem();

	// Model shit
	std::vector<float> verts, coord;
	InitManager::getCube(verts, coord);
	Model m(shader, texture, verts, verts, coord);

	// Simple box example
	std::vector<Entity> entityList;
	unsigned int reserveNum = 465;
	entityList.reserve(reserveNum);
	for (unsigned int i = 0; i < reserveNum; i++) {
		entityList.emplace_back(Entity("box", m, physicsSystem->transformList[i]));
	}


	// Main loop
	while (!window.shouldClose()) {
		window.clear();
		timer.tick();
		input.poll();

		// Use fixed time steps for updates
		while (timer.getAccumultor() >= timer.dt) {
			physicsSystem->updatePhysics(timer.dt, entityList);
			timer.advance();
		}

		// set rotation
		shader.use();

		glm::mat4 projection = glm::perspective(
			glm::radians(camera.Zoom),
			float(window.getWidth()) / float(window.getHeight()),
			0.1f,
			100.0f
		);
		shader.setMat4("projection", projection);

		// camera / view transformation
		glm::mat4 view = camera.GetViewMatrix();
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

			entityList.at(i).model.draw();
		}

		arial.render("FPS" + std::to_string(timer.getFPS()), 10.f, 1390.f, 1.f, glm::vec3(0.5f, 0.8f, 0.2f));

		glfwSwapBuffers(window);
	}

	return 0;
}
