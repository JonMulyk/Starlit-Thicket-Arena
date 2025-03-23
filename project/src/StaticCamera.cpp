#include "Camera.h"
#include "StaticCamera.h"


StaticCamera::StaticCamera(TimeSeconds& t, glm::vec3 position, glm::vec3 front, glm::vec3 up)
    : Camera(t, position, front, up, -90.0f, 0.0f)
{
}

glm::mat4 StaticCamera::GetViewMatrix()
{
	//return glm::lookAt(this->getPosition(), this->Up, this->Front);
	return glm::lookAt(this->getPosition(), this->getPosition() + this->Front, this->Up);
}
