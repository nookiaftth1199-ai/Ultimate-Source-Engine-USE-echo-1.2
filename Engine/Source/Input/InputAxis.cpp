// ============================================================
// Ultimate Source Engine - Input Axis Implementation
// ============================================================

#include "stdafx.h"
#include "InputAxis.h"
#include "GamepadDevice.h"
#include <algorithm>
#include <cmath>

namespace USE {

    InputAxis::InputAxis(const std::string& name)
        : m_name(name)
        , m_rawValue(0.0f)
        , m_value(0.0f)
        , m_smoothing(0.5f)      // moderate smoothing
        , m_deadzone(0.1f)
        , m_invert(false)
    {
    }

    void InputAxis::AddBinding(const InputBinding& binding)
    {
        m_bindings.push_back(binding);
    }

    void InputAxis::RemoveBinding(const InputBinding& binding)
    {
        auto it = std::find_if(m_bindings.begin(), m_bindings.end(),
            [&binding](const InputBinding& b) {
                if (b.type != binding.type) return false;
                switch (b.type) {
                    case InputSourceType::Keyboard:
                        return b.keyCode == binding.keyCode;
                    case InputSourceType::MouseButton:
                        return b.mouseButton == binding.mouseButton;
                    case InputSourceType::GamepadButton:
                        return b.gamepadButton == binding.gamepadButton &&
                               b.gamepadIndex == binding.gamepadIndex;
                    case InputSourceType::GamepadAxis:
                        return b.gamepadAxis == binding.gamepadAxis &&
                               b.gamepadIndex == binding.gamepadIndex;
                    default:
                        return false;
                }
            });
        if (it != m_bindings.end())
            m_bindings.erase(it);
    }

    void InputAxis::ClearBindings()
    {
        m_bindings.clear();
    }

    void InputAxis::Update(const Uint8* keyboardState,
                           int mouseDeltaX, int mouseDeltaY,
                           const std::vector<GamepadDevice*>& gamepads)
    {
        float combined = 0.0f;

        for (const auto& binding : m_bindings) {
            float val = 0.0f;

            switch (binding.type) {
                case InputSourceType::Keyboard: {
                    // Digital key gives +1 or -1 based on binding scale? We'll assume the binding
                    // already encodes direction via a separate scale field (not present yet).
                    // For simplicity, digital keys give +1 when pressed. For an axis, you might
                    // want two keys (e.g., left/right). To keep it simple, we'll ignore digital
                    // keys for axes, or treat them as ±1 if a scale is provided. We'll skip for now.
                    // In a real system, you'd have separate binding fields for scale/direction.
                    break;
                }
                case InputSourceType::MouseButton: {
                    // Mouse button as axis? Not typical; ignore.
                    break;
                }
                case InputSourceType::GamepadButton: {
                    // Gamepad button as axis: pressed gives +1
                    int idx = binding.gamepadIndex;
                    if (idx >= 0 && idx < (int)gamepads.size() && gamepads[idx] && gamepads[idx]->IsConnected()) {
                        val = gamepads[idx]->IsButtonDown(static_cast<GamepadButton>(binding.gamepadButton)) ? 1.0f : 0.0f;
                    }
                    break;
                }
                case InputSourceType::GamepadAxis: {
                    int idx = binding.gamepadIndex;
                    if (idx >= 0 && idx < (int)gamepads.size() && gamepads[idx] && gamepads[idx]->IsConnected()) {
                        val = gamepads[idx]->GetAxis(static_cast<GamepadAxis>(binding.gamepadAxis));
                        if (binding.invert) val = -val;
                        if (fabs(val) >= binding.deadzone) {
                            // Keep as is; deadzone applied at binding level
                        } else {
                            val = 0.0f;
                        }
                    }
                    break;
                }
                case InputSourceType::MouseAxisX: {
                    // Not yet defined in InputBinding; we'll need to extend InputBinding to support mouse axes.
                    // For now, we'll just use mouseDeltaX for a special binding type.
                    // This is a placeholder; real implementation would have a MouseAxis binding type.
                    val = mouseDeltaX * 0.01f; // crude sensitivity
                    break;
                }
                case InputSourceType::MouseAxisY: {
                    val = mouseDeltaY * 0.01f;
                    break;
                }
                default:
                    break;
            }

            // Combine by taking the maximum absolute value (typical for axes)
            if (fabs(val) > fabs(combined))
                combined = val;
        }

        // Apply global deadzone and smoothing
        if (fabs(combined) < m_deadzone)
            combined = 0.0f;

        if (m_smoothing > 0.0f) {
            // Exponential moving average
            m_rawValue = combined;
            m_value = m_value * (1.0f - m_smoothing) + combined * m_smoothing;
        } else {
            m_rawValue = combined;
            m_value = combined;
        }

        if (m_invert)
            m_value = -m_value;
    }

} // namespace USE