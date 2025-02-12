#pragma once

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define NOMINMAX
#include <windows.h>
#include <XInput.h>

#include "PhysicsSystem.h"
#include "Camera.h"
#include "Windowing.h"
#include "TimeSeconds.h"
#include <GLFW/glfw3.h>

#pragma comment(lib, "xinput.lib")

class Controller
{
public:
    Controller(UINT id, Command& command); //default contstuctor with id, uses default deadzone values
    Controller(UINT id, float deadzoneX, float deadzoneY, Command& command); //constructor for custom deadzone

    inline UINT getControllerID() const;
    XINPUT_GAMEPAD* getController();
    bool isConnected();
    bool Update();
    void Vibrate(USHORT leftSpeed, USHORT rightSpeed);
    void Vibrate(USHORT speed);
    void resetVibration();
    bool isButtonPressed(UINT button) const;
	bool isButtonReleased(UINT button) const;
    UINT buttodID();

    float leftStickX, leftStickY, rightStickX, rightStickY;
    float leftTrigger, rightTrigger;

private:
    Command& r_command;
    UINT controllerID;
    XINPUT_STATE state;
    XINPUT_VIBRATION vibration;

    const float maxValue = 1.0f;

    float deadzoneX;
    float deadzoneY;

    float ApplyDeadzone(float value, float maxValue, float deadzone);
};

#endif