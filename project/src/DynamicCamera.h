#pragma once
#include "Camera.h"
#include "GameState.h"

class DynamicCamera : public Camera
{
	private:
		GameState& gameState;

	public:
		DynamicCamera(GameState& gameState, TimeSeconds& t,
			glm::vec3 position = glm::vec3(0.0f, 10.0f, 10.0f),
			glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = -90.0f, float pitch = 0.0f);

		glm::mat4 GetViewMatrix() override;

		void updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight) override;
};
