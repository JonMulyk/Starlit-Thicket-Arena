#include "Controller.h"
#include <algorithm>
#include <climits>

float normalize(float input, float min, float max);

Controller::Controller(UINT id, Camera& camera, Command& command)
	: controllerID(id), r_command(command), r_camera(camera),
deadzoneX(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
deadzoneY(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
{
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
}

Controller::Controller(UINT id, Camera& camera, float deadzoneX, float deadzoneY, Command& command)
	: controllerID(id), deadzoneX(deadzoneX), deadzoneY(deadzoneY), r_command(command), r_camera(camera)
{
    ZeroMemory(&state, sizeof(XINPUT_STATE));
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
}


UINT Controller::getControllerID() const
{
    return controllerID;
}
XINPUT_GAMEPAD* Controller::getController()
{
    return &state.Gamepad;
}

bool Controller::isConnected()
{
    if (XInputGetState(controllerID - 1, &state) == ERROR_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

float normalize(float input, float min, float max)
{
    float average = (min + max) / 2;
    float range = (max - min) / 2;
    return (input - average) / range;
}

float Controller::ApplyDeadzone(float value, float maxValue, float deadzone)
{
    if (value < -deadzone)
    {
        value += deadzone;
    }
    else if (value > deadzone)
    {
        value -= deadzone;
    }
    else
    {
        return 0;
    }
    float normValue = (float)value / (float)(maxValue - deadzone);//scales to 0-1
    return std::max(-1.0f, std::min(normValue, 1.0f));
}


bool Controller::Update()
{
    if (!isConnected())
        return false;

    float normLX = normalize(static_cast<float>(state.Gamepad.sThumbLX), -32767, 32767);
    float normLY = normalize(static_cast<float>(state.Gamepad.sThumbLY), -32767, 32767);

    float normRX = normalize(static_cast<float>(state.Gamepad.sThumbRX), -32767, 32767);
    float normRY = normalize(static_cast<float>(state.Gamepad.sThumbRY), -32767, 32767);

    if (deadzoneX <= 1.0f || deadzoneY <= 1.0f)
    {
        leftStickX = ApplyDeadzone(normLX, maxValue, deadzoneX);
        leftStickY = ApplyDeadzone(normLY, maxValue, deadzoneY);
        rightStickX = ApplyDeadzone(normRX, maxValue, deadzoneX);
        rightStickY = ApplyDeadzone(normRY, maxValue, deadzoneY);
    }
    else
    {
        leftStickX = ApplyDeadzone(normLX, maxValue, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
        leftStickY = ApplyDeadzone(normLY, maxValue, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));
        rightStickX = ApplyDeadzone(normRX, maxValue, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
        rightStickY = ApplyDeadzone(normRY, maxValue, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));
    }

    leftTrigger = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;//normalize input 
    rightTrigger = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;

    //car movement
	r_command.throttle = rightTrigger;
	r_command.brake = leftTrigger;
	if (leftStickX != 0.0f) { r_command.steer = -leftStickX; }
	else { r_command.steer = 0.0f; }

    //camera control
	if (rightStickX != 0.0f || rightStickY != 0.0f)
	{
		r_camera.ProcessMouseMovement(rightStickX*60, rightStickY*60);
	}


    return true;
}


void Controller::Vibrate(USHORT leftSpeed, USHORT rightSpeed)
{
    vibration.wLeftMotorSpeed = leftSpeed;
    vibration.wRightMotorSpeed = rightSpeed;
    XInputSetState(controllerID - 1, &vibration);
}

void Controller::Vibrate(USHORT speed)
{
    vibration.wLeftMotorSpeed = speed;
    vibration.wRightMotorSpeed = speed;
    XInputSetState(controllerID - 1, &vibration);
}

void Controller::resetVibration()
{
    vibration.wLeftMotorSpeed = 0;
    vibration.wRightMotorSpeed = 0;
    XInputSetState(controllerID - 1, &vibration);
}

bool Controller::isButtonPressed(UINT button) const
{
    return (state.Gamepad.wButtons & button) != 0;
}

bool Controller::isButtonReleased(UINT button) const
{
	return (state.Gamepad.wButtons & button) == 0;
}

UINT Controller::buttodID() {
    return state.Gamepad.wButtons;
}

bool Controller::isButtonJustReleased(UINT button) {
    bool wasPressed = (prevButtonState & button) != 0;
    bool isReleased = (state.Gamepad.wButtons & button) == 0;
    prevButtonState = state.Gamepad.wButtons; // Update for next frame
    return wasPressed && isReleased;
}