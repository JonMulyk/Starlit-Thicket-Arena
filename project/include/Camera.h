#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
private:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool FreeCam;
public:
    // Default camera values
    static const float YAW;
    static const float PITCH;
    static const float SPEED;
    static const float SENSITIVITY;
    static const float ZOOM;
    static const glm::vec3 POSITION;
    static const bool FREECAM;

    enum Camera_Inputs {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        TOGGLEFREECAM
    };

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    Camera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    float getZoom() const;
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Inputs direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    // updates camera position
    void updateCameraPosition(glm::vec3 pos);

    // return free camera state
    bool isFreeCam();
};


