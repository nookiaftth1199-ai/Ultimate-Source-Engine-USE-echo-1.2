// ============================================================
// Ultimate Source Engine - Gamepad Device
// ============================================================
//
// Represents a gamepad/controller input device.
// Provides methods to query axis and button states.
// Supports rumble (if available).
// ============================================================

#pragma once

#include "stdafx.h"
#include "InputDevice.h"
#include <SDL.h>
#include <array>

namespace USE {

    // Gamepad axis indices (matches SDL_GameControllerAxis)
    enum class GamepadAxis {
        LeftX = SDL_CONTROLLER_AXIS_LEFTX,
        LeftY = SDL_CONTROLLER_AXIS_LEFTY,
        RightX = SDL_CONTROLLER_AXIS_RIGHTX,
        RightY = SDL_CONTROLLER_AXIS_RIGHTY,
        LeftTrigger = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
        RightTrigger = SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
        Count
    };

    // Gamepad button indices (matches SDL_GameControllerButton)
    enum class GamepadButton {
        A = SDL_CONTROLLER_BUTTON_A,
        B = SDL_CONTROLLER_BUTTON_B,
        X = SDL_CONTROLLER_BUTTON_X,
        Y = SDL_CONTROLLER_BUTTON_Y,
        Back = SDL_CONTROLLER_BUTTON_BACK,
        Guide = SDL_CONTROLLER_BUTTON_GUIDE,
        Start = SDL_CONTROLLER_BUTTON_START,
        LeftStick = SDL_CONTROLLER_BUTTON_LEFTSTICK,
        RightStick = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
        LeftShoulder = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
        RightShoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
        DPadUp = SDL_CONTROLLER_BUTTON_DPAD_UP,
        DPadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        DPadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        DPadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        Misc1 = SDL_CONTROLLER_BUTTON_MISC1,    // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button
        RightPaddle1 = SDL_CONTROLLER_BUTTON_RIGHT_PADDLE1,
        LeftPaddle1 = SDL_CONTROLLER_BUTTON_LEFT_PADDLE1,
        RightPaddle2 = SDL_CONTROLLER_BUTTON_RIGHT_PADDLE2,
        LeftPaddle2 = SDL_CONTROLLER_BUTTON_LEFT_PADDLE2,
        Touchpad = SDL_CONTROLLER_BUTTON_TOUCHPAD,
        Misc2 = SDL_CONTROLLER_BUTTON_MISC2,
        Misc3 = SDL_CONTROLLER_BUTTON_MISC3,
        Misc4 = SDL_CONTROLLER_BUTTON_MISC4,
        Count
    };

    class GamepadDevice : public InputDevice {
    public:
        // Constructor: takes SDL joystick index (0..) – opens the game controller.
        GamepadDevice(int deviceIndex);
        ~GamepadDevice();

        // Initialize – opens the controller. Returns true if successful.
        bool Initialize();

        // Update device state (called once per frame)
        void Update() override;

        // Check if device is connected
        bool IsConnected() const override { return m_controller != nullptr; }

        // Get device name
        std::string GetName() const override { return m_name; }

        // Get device type
        InputDeviceType GetType() const override { return InputDeviceType::Gamepad; }

        // Get unique identifier (joystick instance ID)
        int GetDeviceID() const override { return m_instanceId; }

        // Axis queries
        float GetAxis(GamepadAxis axis) const;          // returns -1..1 (or 0..1 for triggers)
        float GetAxisRaw(GamepadAxis axis) const;       // returns raw -32768..32767

        // Button queries
        bool IsButtonDown(GamepadButton button) const;
        bool IsButtonPressed(GamepadButton button) const;  // just pressed this frame
        bool IsButtonReleased(GamepadButton button) const; // just released this frame

        // Rumble (vibration)
        bool SetRumble(float lowFrequency, float highFrequency, Uint32 durationMs);

    private:
        SDL_GameController* m_controller;
        int                 m_deviceIndex;      // SDL joystick index
        SDL_JoystickID      m_instanceId;       // SDL instance ID (for event matching)
        std::string         m_name;

        // Axis state (float normalized -1..1)
        std::array<float, static_cast<int>(GamepadAxis::Count)> m_axisValues;

        // Button state (current and previous)
        std::array<Uint8, static_cast<int>(GamepadButton::Count)> m_currentButtons;
        std::array<Uint8, static_cast<int>(GamepadButton::Count)> m_previousButtons;
    };

} // namespace USE