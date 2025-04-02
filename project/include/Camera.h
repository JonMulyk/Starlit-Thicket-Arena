#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TimeSeconds.h"
#include "GameState.h"
#include "Shader.h"


class Camera {
private:
	Transform* followTarget = nullptr;
protected:
    bool isStaticCam = false;
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    float Theta;
    float Phi;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    TimeSeconds& timer;
    double time;
    double factor = 10;

    Camera(TimeSeconds& timer, glm::vec3 position, glm::vec3 front, glm::vec3 up,
        float yaw, float pitch, float theta = 0.0f, float phi = 0.0f);

public:
    // Default camera values
    static const float YAW;
    static const float PITCH;
    static const float SPEED;
    static const float SENSITIVITY;
    static const float ZOOM;
    static const float THETA;
    static const float PHI;

    enum class Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };


    virtual ~Camera() = default;
    
    float getZoom() const;
    const glm::vec3 getPosition() const;
    virtual glm::mat4 GetViewMatrix() = 0;


    virtual void updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight) = 0;
    //void updateProjectionView(Shader& viewShader, int windowWidth, int windowHeight);


    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    //
    void incrementTheta(float dt);

    void incrementPhi(float dp);

    // Linear interpolation function
    float lerp(float a, float b, float t) { return a + t * (b - a);}

    void updateZoom(float speed);
    void updateYawWithDelay(float targetYaw, float deltaTime);
};


