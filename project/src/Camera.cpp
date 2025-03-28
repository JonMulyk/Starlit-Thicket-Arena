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

Camera::Camera(TimeSeconds& timer, glm::vec3 position, glm::vec3 front, glm::vec3 up, 
    float yaw, float pitch, float theta, float phi) 
	: timer(timer),
	Position(position),
    Front(front),
	Up(up),
	Right(glm::vec3(0.0f, 0.0f, 0.0f)),
    WorldUp(up),
    Yaw(yaw),
    Pitch(pitch),
    Theta(theta),
    Phi(phi),
	MovementSpeed(SPEED),
	MouseSensitivity(SENSITIVITY),
	Zoom(ZOOM)
{
    time = timer.getCurrentTime();
}


float Camera::getZoom() const {
    return Zoom;
}

const glm::vec3 Camera::getPosition() const
{
    return this->Position;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera::Camera_Movement::FORWARD)
        Position += Front * velocity;
    if (direction == Camera::Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    if (direction == Camera::Camera_Movement::LEFT)
        Position -= Right * velocity;
    if (direction == Camera::Camera_Movement::RIGHT)
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
    if (Theta + static_cast<float>(dt / 100.0f) < static_cast<float>(M_PI_2) && 
        Theta + static_cast<float>(dt / 100.0f) > static_cast<float>(-M_PI_2)) 
    {
        Theta += static_cast<float>(dt / 100.0f);
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

void Camera::updateZoom(float speed) {
    // Compute the target zoom based on the speed
    float targetZoom = (speed * 3.5) + 40.f;
    targetZoom = targetZoom - 5;

    // interpolation factor (t) between 0 and 1.
    float t = 0.1f;

    // lock zoom above 45
	if (targetZoom < 55.0f) {
		targetZoom = 55.0f;
	}
    if (targetZoom > 80.0f) {
		targetZoom = 80.0f;
    }
	std::cout << "targetZoom: " << targetZoom << std::endl;
    // Smoothly interpolate from the current zoom to the target zoom
    Zoom = lerp(Zoom, targetZoom, t);
}

void Camera::updateYawWithDelay(float targetYaw, float deltaTime) {
    // Smoothing factor: higher values mean a faster response; adjust as needed.
    const float smoothingFactor = 5.0f;

    // Interpolate current yaw toward the target yaw.
    // This formula implements a simple exponential smoothing.
    Yaw += (targetYaw - Yaw) * smoothingFactor * deltaTime;

    // Update the camera vectors based on the new yaw value.
    updateCameraVectors();
}
