// ============================================================
// Ultimate Source Engine - Input Action
// ============================================================
//
// Represents a named game action (e.g., "MoveForward", "Jump")
// that can be bound to multiple physical inputs (keyboard,
// mouse, gamepad). Provides methods to query the current value
// (0..1 for digital, -1..1 for analog) and detect state changes.
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

    class InputAction {
    public:
        explicit InputAction(const std::string& name);
        ~InputAction() = default;

        // Name of the action
        const std::string& GetName() const { return m_name; }

        // Bindings management
        void AddBinding(const InputBinding& binding);
        void RemoveBinding(const InputBinding& binding); // optional
        void ClearBindings();

        // Update the action's current value. Called by InputManager.
        // Parameters: keyboard state array, mouse button mask,
        // mouse position/delta (optional), and array of connected gamepads.
        void Update(const Uint8* keyboardState,
                    Uint32 mouseState,
                    const std::vector<GamepadDevice*>& gamepads);

        // Current value (0..1 for digital, -1..1 for analog)
        float GetValue() const { return m_value; }

        // Previous frame's value (for edge detection)
        float GetPreviousValue() const { return m_prevValue; }

        // Convenient state queries with optional threshold
        bool IsPressed(float threshold = 0.5f) const { return m_value > threshold; }
        bool WasPressedThisFrame(float threshold = 0.5f) const {
            return m_value > threshold && m_prevValue <= threshold;
        }
        bool WasReleasedThisFrame(float threshold = 0.5f) const {
            return m_value <= threshold && m_prevValue > threshold;
        }

    private:
        std::string m_name;
        std::vector<InputBinding> m_bindings;
        float m_value;      // current frame value
        float m_prevValue;  // previous frame value
    };

} // namespace USE