#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL //NOTE: glm will throw error messages if this line is not included
#include<glm/gtx/quaternion.hpp>


class Transform {
public:
	glm::vec3 scale = glm::vec3(1.f);
	glm::vec3 pos;
	glm::quat rot;

	Transform();

	void updateUniformScale(float s);

	glm::mat4 getModel();
};