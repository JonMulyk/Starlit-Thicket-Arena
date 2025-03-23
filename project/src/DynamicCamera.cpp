#include "DynamicCamera.h"

#define _USE_MATH_DEFINES
#include <math.h>

DynamicCamera::DynamicCamera(
    GameState& gameState, TimeSeconds& t, glm::vec3 position, 
    glm::vec3 front, glm::vec3 up, float yaw, float pitch)
    : Camera(t, position, front, up, yaw, pitch), gameState(gameState)
{
}

glm::vec3 vec3(physx::PxVec3 v) 
{ 
    return glm::vec3(v.x, v.y, v.z); 
}

glm::mat4 DynamicCamera::GetViewMatrix()
{
    glm::vec3 pos = vec3(gameState.playerVehicle.curPos);
    glm::vec3 dir = vec3(gameState.playerVehicle.curDir);

    if (timer.getCurrentTime() - time > 1.f) {
        double threshold = 0.1f;
        //double factor2 = 100.f * timer.getFrameTime();
        double factor2 = factor * timer.getFrameTime();
        if (Phi > 2 * M_PI - threshold || Phi < threshold) {
            Phi = 0;
        }
        else if (Phi < M_PI) {
            incrementPhi(factor2);
        }
        else {
            incrementPhi(-factor2);
        }
        factor += timer.getFrameTime() * 50;
    }
    glm::vec4 rot = glm::rotate(glm::mat4(1.f), Phi, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(dir, 0.f);
    dir = glm::normalize(glm::vec3(rot));

    pos.y += 4.f;

    return glm::lookAt(pos - 10.f * dir, pos + 2.f * dir, glm::vec3(0.0f, 1.0f, 0.0f));
}

void DynamicCamera::updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight)
{
    viewShader.use();

    glm::mat4 projection = glm::perspective(
        glm::radians(this->getZoom()),
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
        0.1f, 1000.0f
    );
    viewShader.setMat4("projection", projection);

    glm::mat4 view = this->GetViewMatrix();
    viewShader.setMat4("view", view);
}

