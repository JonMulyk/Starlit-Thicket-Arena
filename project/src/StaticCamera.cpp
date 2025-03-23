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

void StaticCamera::updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight)
{
    viewShader.use();
/*
    glm::mat4 projection = glm::perspective(
        glm::radians(this->getZoom()),
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
        0.1f, 1000.0f
    );
    */
    // Orthographic projection for minimap
    float scale = 100.0f;
    float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    glm::mat4 projection = glm::ortho(
        -scale * aspect,        // left
        scale * aspect,         // right
        -scale,                 // bottom
        scale,                  // top
        -1000.0f, 1000.0f       // zNear and zFar plane
    );

    viewShader.setMat4("projection", projection);

    glm::mat4 view = this->GetViewMatrix();
    viewShader.setMat4("view", view);
}

