#pragma once
#include "Camera.h"

class StaticCamera : public Camera
{
	private:

	public:
		StaticCamera(TimeSeconds& t, glm::vec3 position, glm::vec3 front, glm::vec3 up);

		glm::mat4 GetViewMatrix() override;

		void updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight) override;
};

