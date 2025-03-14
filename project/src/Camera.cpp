#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


const float Camera::YAW = -90.0f;
const float Camera::PITCH = 0.0f;
const float Camera::SPEED = 10.f;
const float Camera::SENSITIVITY = 0.1f;
const float Camera::ZOOM = 45.0f;
const float Camera::THETA = 0.0f;
const float Camera::PHI = 0.0f;

// code does not seem necessary
/*
Camera::Camera(
    float posX, float posY, float posZ,
    float upX, float upY, float upZ,
    float yaw, float pitch, float theta, float phi,
    GameState& gameState
) :
    gState(gameState),
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ); // Position as seen in above diagram
    WorldUp = glm::vec3(upX, upY, upZ); // Up vector as seen in above diagram
    Yaw = yaw;
    Pitch = pitch;
    Theta = theta;

    updateCameraVectors();
}
*/

Camera::Camera(GameState& gameState, TimeSeconds& t, glm::vec3 position, glm::vec3 up, float yaw, float pitch, float theta, float phi) :
    gState(gameState),
    timer(t),
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
{
    time = timer.getCurrentTime();
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Theta = theta;
    Phi = phi;

    //updateCameraVectors();
}


float Camera::getZoom() const {
    return Zoom;
}

const glm::vec3 Camera::getPosition() const
{
    return this->Position;
}

glm::vec3 vec3(physx::PxVec3 v) { return glm::vec3(v.x, v.y, v.z); }

glm::mat4 Camera::GetViewMatrix() {
    glm::vec3 pos = vec3(gState.playerVehicle.curPos);
    glm::vec3 dir = vec3(gState.playerVehicle.curDir);

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

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    time = timer.getCurrentTime();
    factor = 1;
    incrementTheta(-yoffset);
    incrementPhi(xoffset);

    // Freecam controls
    /*
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    Yaw = 10.f;

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
    */
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::incrementTheta(float dt) {
    if (Theta + (dt / 100.0f) < M_PI_2 && Theta + (dt / 100.0f) > -M_PI_2) {
        Theta += dt / 100.0f;
    }
}

void Camera::incrementPhi(float dp) {
    Phi -= dp / 100.0f;

    if (Phi > 2.0 * M_PI) {
        Phi -= 2.0 * M_PI;
    }
    else if (Phi < 0.0f) {
        Phi += 2.0 * M_PI;
    }
}

// Linear interpolation function
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void Camera::updateZoom(float speed) {
    // Compute the target zoom based on the speed
    float targetZoom = (speed * 3.5) + 40.f;

    // Define an interpolation factor (t) between 0 and 1.
    // A smaller value means a smoother (slower) transition.
    float t = 0.1f; // Adjust this factor as needed

    // Smoothly interpolate from the current zoom to the target zoom
    Zoom = lerp(Zoom, targetZoom, t);
}