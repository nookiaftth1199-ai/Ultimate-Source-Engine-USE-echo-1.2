// ============================================================
// Ultimate Source Engine - Input Axis
// ============================================================
//
// Represents an analog input axis (e.g., movement, look) that
// can be bound to multiple physical inputs (gamepad sticks,
// mouse movement, keyboard keys as digital‑to‑analog).
// Provides smoothing, deadzone, and inversion.
// ============================================================

#pragma once

#include "stdafx.h"
#include "InputBinding.h"
#include <SDL.h>
#include <string>
#include <vector>

namespace USE {

    // Forward declarations
    class GamepadDevice;

    class InputAxis {
    public:
        explicit InputAxis(const std::string& name);
        ~InputAxis() = default;

        // Name of the axis
        const std::string& GetName() const { return m_name; }

        // Bindings management
        void AddBinding(const InputBinding& binding);
        void RemoveBinding(const InputBinding& binding);
        void ClearBindings();

        // Update the axis value from input sources.
        // Parameters: keyboard state, mouse delta/position,
        // and connected gamepads.
        void Update(const Uint8* keyboardState,
                    int mouseDeltaX, int mouseDeltaY,
                    const std::vector<GamepadDevice*>& gamepads);

        // Current value (after smoothing, deadzone, etc.)
        float GetValue() const { return m_value; }

        // Raw value (before smoothing)
        float GetRawValue() const { return m_rawValue; }

        // Smoothing factor (0 = no smoothing, 1 = instant)
        void SetSmoothing(float factor) { m_smoothing = factor; }
        float GetSmoothing() const { return m_smoothing; }

        // Global deadzone (applied after combining inputs)
        void SetDeadzone(float deadzone) { m_deadzone = deadzone; }
        float GetDeadzone() const { return m_deadzone; }

        // Invert the final value
        void SetInvert(bool invert) { m_invert = invert; }
        bool GetInvert() const { return m_invert; }

    private:
        std::string m_name;
        std::vector<InputBinding> m_bindings;

        float m_rawValue;    // combined raw input (before smoothing/deadzone)
        float m_value;       // smoothed, deadzone‑applied final value

        float m_smoothing;   // exponential smoothing factor (0–1)
        float m_deadzone;    // value below which output is zero
        bool  m_invert;      // invert final output
    };

} // namespace USE