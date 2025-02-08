#pragma once


/*
Code implemented based on tutorial from:
https://lcmccauley.wordpress.com/2014/01/05/gamepad-input-tutorial/
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601  // Windows 7 or later
#endif

#include <windows.h>
#include <winnt.h>
#include <interlockedapi.h>
#include <minwinbase.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>


#include <Xinput.h>
#include <Windows.h>


// XInput Button values
static const WORD XINPUT_Buttons[] = {
      XINPUT_GAMEPAD_A,
      XINPUT_GAMEPAD_B,
      XINPUT_GAMEPAD_X,
      XINPUT_GAMEPAD_Y,
      XINPUT_GAMEPAD_DPAD_UP,
      XINPUT_GAMEPAD_DPAD_DOWN,
      XINPUT_GAMEPAD_DPAD_LEFT,
      XINPUT_GAMEPAD_DPAD_RIGHT,
      XINPUT_GAMEPAD_LEFT_SHOULDER,
      XINPUT_GAMEPAD_RIGHT_SHOULDER,
      XINPUT_GAMEPAD_LEFT_THUMB,
      XINPUT_GAMEPAD_RIGHT_THUMB,
      XINPUT_GAMEPAD_START,
      XINPUT_GAMEPAD_BACK
};

// XInput Button IDs
struct XButtonIDs
{
    // Function prototypes
    //---------------------//

    XButtonIDs(); // Default constructor

    // Member variables
    //---------------------//

    int A, B, X, Y; // 'Action' buttons

    // Directional Pad (D-Pad)
    int DPad_Up, DPad_Down, DPad_Left, DPad_Right;

    // Shoulder ('Bumper') buttons
    int L_Shoulder, R_Shoulder;

    // Thumbstick buttons
    int L_Thumbstick, R_Thumbstick;

    int Start; // 'START' button
    int Back;  // 'BACK' button
};

class Gamepad
{
public:
    Gamepad();
    Gamepad(int a_iIndex);

    void Update();       // Update gamepad state
    void RefreshState(); // Update button states for next frame

    float LeftStick_X();
    float LeftStick_Y(); 
    float RightStick_X();
    float RightStick_Y(); 

    float LeftTrigger();
    float RightTrigger(); 

    bool GetButtonPressed(int a_iButton);
    bool GetButtonDown(int a_iButton);

    XINPUT_STATE GetState();
    int GetIndex();
    bool Connected();

    // Vibrate the gamepad (0.0f cancel, 1.0f max speed)
    void Rumble(float a_fLeftMotor = 0.0f, float a_fRightMotor = 0.0f);

private:
    XINPUT_STATE m_State;
    int m_iGamepadIndex; 

    static const int ButtonCount = 14;
    bool bPrev_ButtonStates[ButtonCount];
    bool bButtonStates[ButtonCount];

    bool bGamepad_ButtonsDown[ButtonCount];
};

#endif // GAMEPAD_H

extern XButtonIDs XButtons;