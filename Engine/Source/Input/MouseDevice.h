// ============================================================
// Ultimate Source Engine - Mouse Device
// ============================================================
//
// Represents a mouse input device.
// Provides methods to query button states, position, and wheel.
// ============================================================

#pragma once

#include "stdafx.h"
#include "InputDevice.h"
#include <SDL.h>

namespace USE {

    // Mouse button constants (match SDL)
    enum MouseButton {
        MouseButton_Left = SDL_BUTTON_LEFT,
        MouseButton_Middle = SDL_BUTTON_MIDDLE,
        MouseButton_Right = SDL_BUTTON_RIGHT,
        MouseButton_X1 = SDL_BUTTON_X1,
        MouseButton_X2 = SDL_BUTTON_X2
    };

    class MouseDevice : public InputDevice {
    public:
        MouseDevice();
        ~MouseDevice();

        // Initialize
        bool Initialize();

        // Update device state (called once per frame)
        void Update() override;

        // Check if device is connected/available (always true for mouse)
        bool IsConnected() const override { return true; }

        // Get device name
        std::string GetName() const override { return "Mouse"; }

        // Get device type
        InputDeviceType GetType() const override { return InputDeviceType::Mouse; }

        // Button state queries
        bool IsButtonDown(int button) const;
        bool IsButtonPressed(int button) const;  // just pressed this frame
        bool IsButtonReleased(int button) const; // just released this frame

        // Position
        void GetPosition(int& x, int& y) const;
        int GetX() const { return m_x; }
        int GetY() const { return m_y; }

        // Delta (movement since last frame)
        void GetDelta(int& dx, int& dy) const;
        int GetDeltaX() const { return m_deltaX; }
        int GetDeltaY() const { return m_deltaY; }

        // Wheel
        int GetWheelDelta() const { return m_wheelDelta; }

    private:
        Uint32 m_currentState;   // current frame button mask
        Uint32 m_previousState;  // previous frame button mask
        int    m_x, m_y;          // current position
        int    m_deltaX, m_deltaY; // movement delta
        int    m_wheelDelta;      // accumulated wheel movement this frame
    };

} // namespace USE