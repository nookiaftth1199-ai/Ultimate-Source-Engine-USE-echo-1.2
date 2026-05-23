// ============================================================
// Ultimate Source Engine - Keyboard Device
// ============================================================
//
// Represents a keyboard input device.
// Provides methods to query key states.
// ============================================================

#pragma once

#include "stdafx.h"
#include "InputDevice.h"
#include <SDL.h>
#include <vector>

namespace USE {

    class KeyboardDevice : public InputDevice {
    public:
        KeyboardDevice();
        ~KeyboardDevice();

        // Initialize (loads initial keyboard state)
        bool Initialize();

        // Update device state (called once per frame)
        void Update() override;

        // Check if device is connected/available (always true for keyboard)
        bool IsConnected() const override { return true; }

        // Get device name
        std::string GetName() const override { return "Keyboard"; }

        // Get device type
        InputDeviceType GetType() const override { return InputDeviceType::Keyboard; }

        // Key state queries
        bool IsKeyDown(SDL_Scancode key) const;
        bool IsKeyPressed(SDL_Scancode key) const;  // just pressed this frame
        bool IsKeyReleased(SDL_Scancode key) const; // just released this frame

    private:
        const Uint8* m_currentState;   // SDL-owned current frame state
        std::vector<Uint8> m_previousState; // previous frame state (copy)
        int m_stateLength;
    };

} // namespace USE