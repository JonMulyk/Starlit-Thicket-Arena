#pragma once

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define NOMINMAX
#include <windows.h>
#include <XInput.h>

#pragma comment(lib, "xinput.lib")

class Controller
{
public:
    Controller(UINT id);
    Controller(UINT id, float deadzoneX, float deadzoneY);

    inline UINT getControllerID() const;
    XINPUT_GAMEPAD* getController();
    bool isConnected();
    bool Update();
    void Vibrate(USHORT leftSpeed, USHORT rightSpeed);
    void Vibrate(USHORT speed);
    void resetVibration();
    bool isButtonPressed(UINT button) const;

    float leftStickX, leftStickY, rightStickX, rightStickY;
    float leftTrigger, rightTrigger;

private:
    UINT controllerID;
    XINPUT_STATE state;
    XINPUT_VIBRATION vibration;

    const float maxValue = 1.0f;

    float deadzoneX;
    float deadzoneY;

    float ApplyDeadzone(float value, float maxValue, float deadzone);
};

#endif