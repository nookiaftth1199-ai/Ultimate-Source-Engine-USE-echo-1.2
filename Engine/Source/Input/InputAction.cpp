// ============================================================
// Ultimate Source Engine - Input Action Implementation
// ============================================================

#include "stdafx.h"
#include "InputAction.h"
#include "GamepadDevice.h"
#include <algorithm>

namespace USE {

    InputAction::InputAction(const std::string& name)
        : m_name(name)
        , m_value(0.0f)
        , m_prevValue(0.0f)
    {
    }

    void InputAction::AddBinding(const InputBinding& binding)
    {
        m_bindings.push_back(binding);
    }

    void InputAction::RemoveBinding(const InputBinding& binding)
    {
        // Simple linear remove – for production you might use a more robust approach.
        auto it = std::find_if(m_bindings.begin(), m_bindings.end(),
            [&binding](const InputBinding& b) {
                // Compare based on type and union values – simplistic.
                if (b.type != binding.type) return false;
                switch (b.type) {
                    case InputSourceType::Keyboard:
                        return b.keyCode == binding.keyCode;
                    case InputSourceType::MouseButton:
                        return b.mouseButton == binding.mouseButton;
                    case InputSourceType::GamepadButton:
                        return b.gamepadButton == binding.gamepadButton && b.gamepadIndex == binding.gamepadIndex;
                    case InputSourceType::GamepadAxis:
                        return b.gamepadAxis == binding.gamepadAxis && b.gamepadIndex == binding.gamepadIndex;
                    default:
                        return false;
                }
            });
        if (it != m_bindings.end())
            m_bindings.erase(it);
    }

    void InputAction::ClearBindings()
    {
        m_bindings.clear();
    }

    void InputAction::Update(const Uint8* keyboardState,
                             Uint32 mouseState,
                             const std::vector<GamepadDevice*>& gamepads)
    {
        m_prevValue = m_value;
        m_value = 0.0f;

        for (const auto& binding : m_bindings) {
            float val = 0.0f;

            switch (binding.type) {
                case InputSourceType::Keyboard:
                    if (keyboardState && binding.keyCode < SDL_NUM_SCANCODES)
                        val = (keyboardState[binding.keyCode] != 0) ? 1.0f : 0.0f;
                    break;

                case InputSourceType::MouseButton:
                    if (binding.mouseButton > 0)
                        val = (mouseState & SDL_BUTTON(binding.mouseButton)) ? 1.0f : 0.0f;
                    break;

                case InputSourceType::GamepadButton: {
                    int idx = binding.gamepadIndex;
                    if (idx >= 0 && idx < (int)gamepads.size() && gamepads[idx] && gamepads[idx]->IsConnected()) {
                        // Convert int to GamepadButton enum (assumes SDL values match)
                        val = gamepads[idx]->IsButtonDown(static_cast<GamepadButton>(binding.gamepadButton)) ? 1.0f : 0.0f;
                    }
                    break;
                }

                case InputSourceType::GamepadAxis: {
                    int idx = binding.gamepadIndex;
                    if (idx >= 0 && idx < (int)gamepads.size() && gamepads[idx] && gamepads[idx]->IsConnected()) {
                        float raw = gamepads[idx]->GetAxis(static_cast<GamepadAxis>(binding.gamepadAxis));
                        if (binding.invert) raw = -raw;
                        if (fabs(raw) >= binding.deadzone) {
                            val = raw;
                        } else {
                            val = 0.0f;
                        }
                    }
                    break;
                }
            }

            // For digital bindings, val is 0 or 1; for analog, it's in -1..1.
            // We keep the value with the largest absolute magnitude.
            if (fabs(val) > fabs(m_value))
                m_value = val;
        }
    }

} // namespace USE