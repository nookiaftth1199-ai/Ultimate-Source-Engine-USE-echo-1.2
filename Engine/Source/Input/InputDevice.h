// ============================================================
// Ultimate Source Engine - Input Device Interface
// ============================================================
//
// Abstract base class for all input devices (keyboard, mouse,
// gamepad, etc.). Provides a common interface for polling and
// device identification.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    // Device types
    enum class InputDeviceType {
        Keyboard,
        Mouse,
        Gamepad,
        Joystick,
        Touch,
        Other
    };

    class InputDevice {
    public:
        virtual ~InputDevice() = default;

        // Update device state (called once per frame)
        virtual void Update() = 0;

        // Check if device is connected/available
        virtual bool IsConnected() const = 0;

        // Get device name (for display)
        virtual std::string GetName() const = 0;

        // Get device type
        virtual InputDeviceType GetType() const = 0;

        // Optionally, get a unique identifier (e.g., SDL joystick index)
        virtual int GetDeviceID() const { return -1; }
    };

} // namespace USE