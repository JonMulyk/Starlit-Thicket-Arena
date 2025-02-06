#include "Transform.h"


Transform::Transform() : scale(1.f), pos(0.f), rot() {}

void Transform::updateUniformScale(float s) {
	scale = glm::vec3(s, s, s);
}

glm::mat4 Transform::getModel() {
	glm::mat4 model = glm::mat4(1.0f);				// initial matrix
	model = glm::translate(model, pos);				// translate
	model = model * glm::mat4_cast(rot);			// rotate
	model = glm::scale(model, scale);				// scale
	return model;
}
