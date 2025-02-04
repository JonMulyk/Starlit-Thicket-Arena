#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL //NOTE: glm will throw error messages if this line is not included
#include<glm/gtx/quaternion.hpp>


class Transform {
public:
	glm::vec3 scale;
	glm::vec3 pos;
	glm::quat rot;

	Transform() : scale(1.f), pos(0.f), rot() {}

	void updateUniformScale(float s) {
		scale = glm::vec3(s, s, s);
	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);				// initial matrix
		model = glm::translate(model, pos);				// translate
		model = model * glm::mat4_cast(rot);			// rotate
		model = glm::scale(model, scale);				// scale
		return model;
	}
};