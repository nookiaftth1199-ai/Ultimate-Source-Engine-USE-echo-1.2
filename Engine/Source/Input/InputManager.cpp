// ============================================================
// Ultimate Source Engine - Input Manager Implementation
// ============================================================

#include "stdafx.h"
#include "InputManager.h"
#include "Core/Logger.h"
#include <cstring>

namespace USE {

    InputManager::InputManager()
        : m_keyboardState(nullptr)
        , m_previousKeyboardState(nullptr)
        , m_keyboardStateLength(0)
        , m_mouseState(0)
        , m_previousMouseState(0)
        , m_mouseX(0), m_mouseY(0)
        , m_mouseDeltaX(0), m_mouseDeltaY(0)
        , m_mouseWheelDelta(0)
    {
        for (int i = 0; i < MAX_GAMEPADS; ++i) {
            m_gamepads[i] = nullptr;
            memset(m_axisState[i], 0, sizeof(m_axisState[i]));
            memset(m_buttonState[i], 0, sizeof(m_buttonState[i]));
            memset(m_previousButtonState[i], 0, sizeof(m_previousButtonState[i]));
        }
    }

    InputManager::~InputManager()
    {
        Shutdown();
    }

    bool InputManager::Initialize()
    {
        // SDL video must have been initialized already (by window).
        // We just need to open game controllers.
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                SDL_GameController* ctrl = SDL_GameControllerOpen(i);
                if (ctrl) {
                    for (int j = 0; j < MAX_GAMEPADS; ++j) {
                        if (m_gamepads[j] == nullptr) {
                            m_gamepads[j] = ctrl;
                            USE_LOG_INFO("InputManager: Opened gamepad %d", i);
                            break;
                        }
                    }
                }
            }
        }

        // Get keyboard state array (SDL keeps it)
        m_keyboardState = SDL_GetKeyboardState(&m_keyboardStateLength);
        m_previousKeyboardState = new Uint8[m_keyboardStateLength];
        memset(m_previousKeyboardState, 0, m_keyboardStateLength);

        return true;
    }

    void InputManager::Shutdown()
    {
        delete[] m_previousKeyboardState;
        m_previousKeyboardState = nullptr;

        for (int i = 0; i < MAX_GAMEPADS; ++i) {
            if (m_gamepads[i]) {
                SDL_GameControllerClose(m_gamepads[i]);
                m_gamepads[i] = nullptr;
            }
        }
    }

    void InputManager::Update()
    {
        // Save previous keyboard state
        memcpy(m_previousKeyboardState, m_keyboardState, m_keyboardStateLength);

        // Save previous mouse button state
        m_previousMouseState = m_mouseState;

        // Get current mouse state
        m_mouseState = SDL_GetMouseState(&m_mouseX, &m_mouseY);

        // Get relative motion (delta)
        SDL_GetRelativeMouseState(&m_mouseDeltaX, &m_mouseDeltaY);

        // Reset mouse wheel delta (it's set via events in PumpEvents, but we'll accumulate)
        m_mouseWheelDelta = 0;

        // Update gamepad state
        UpdateGamepads();

        // Process bindings
        for (auto& binding : m_keyBindings) {
            bool isDown = IsKeyDown(binding.second.key);
            // For simplicity, we call callback when key is pressed (not every frame)
            // Could add separate "down" vs "pressed" semantics.
            if (isDown) {
                binding.second.callback(1.0f); // value 1.0 for pressed
            }
        }

        for (auto& binding : m_mouseBindings) {
            bool isDown = IsMouseButtonDown(binding.second.button);
            if (isDown) {
                binding.second.callback(1.0f);
            }
        }

        for (auto& binding : m_axisBindings) {
            if (IsGamepadConnected(0)) {
                float val = GetGamepadAxis(0, binding.second.axis);
                if (fabs(val) > binding.second.deadzone) {
                    binding.second.callback(val);
                }
            }
        }
    }

    bool InputManager::IsKeyDown(SDL_Scancode key) const
    {
        return m_keyboardState[key] != 0;
    }

    bool InputManager::IsKeyPressed(SDL_Scancode key) const
    {
        return m_keyboardState[key] != 0 && m_previousKeyboardState[key] == 0;
    }

    bool InputManager::IsKeyReleased(SDL_Scancode key) const
    {
        return m_keyboardState[key] == 0 && m_previousKeyboardState[key] != 0;
    }

    bool InputManager::IsMouseButtonDown(int button) const
    {
        return (m_mouseState & SDL_BUTTON(button)) != 0;
    }

    bool InputManager::IsMouseButtonPressed(int button) const
    {
        return (m_mouseState & SDL_BUTTON(button)) != 0 &&
               (m_previousMouseState & SDL_BUTTON(button)) == 0;
    }

    bool InputManager::IsMouseButtonReleased(int button) const
    {
        return (m_mouseState & SDL_BUTTON(button)) == 0 &&
               (m_previousMouseState & SDL_BUTTON(button)) != 0;
    }

    void InputManager::GetMousePosition(int& x, int& y) const
    {
        x = m_mouseX;
        y = m_mouseY;
    }

    void InputManager::GetMouseDelta(int& dx, int& dy) const
    {
        dx = m_mouseDeltaX;
        dy = m_mouseDeltaY;
    }

    bool InputManager::IsGamepadConnected(int index) const
    {
        return index >= 0 && index < MAX_GAMEPADS && m_gamepads[index] != nullptr;
    }

    float InputManager::GetGamepadAxis(int index, GamepadAxis axis) const
    {
        if (!IsGamepadConnected(index)) return 0.0f;
        return m_axisState[index][axis];
    }

    bool InputManager::IsGamepadButtonDown(int index, int button) const
    {
        if (!IsGamepadConnected(index)) return false;
        return m_buttonState[index][button] != 0;
    }

    void InputManager::UpdateGamepads()
    {
        for (int i = 0; i < MAX_GAMEPADS; ++i) {
            if (!m_gamepads[i]) continue;

            // Save previous button state
            memcpy(m_previousButtonState[i], m_buttonState[i], sizeof(m_buttonState[i]));

            // Read current button state
            for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; ++b) {
                m_buttonState[i][b] = SDL_GameControllerGetButton(m_gamepads[i],
                    (SDL_GameControllerButton)b);
            }

            // Read axes
            m_axisState[i][GamepadAxis_LeftX] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
            m_axisState[i][GamepadAxis_LeftY] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
            m_axisState[i][GamepadAxis_RightX] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
            m_axisState[i][GamepadAxis_RightY] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;
            m_axisState[i][GamepadAxis_LeftTrigger] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
            m_axisState[i][GamepadAxis_RightTrigger] = SDL_GameControllerGetAxis(m_gamepads[i],
                SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
        }
    }

    void InputManager::BindAction(const std::string& actionName, SDL_Scancode key,
                                   InputActionCallback callback)
    {
        KeyBinding kb;
        kb.key = key;
        kb.callback = callback;
        m_keyBindings[actionName] = kb;
    }

    void InputManager::BindAction(const std::string& actionName, int mouseButton,
                                   InputActionCallback callback)
    {
        MouseBinding mb;
        mb.button = mouseButton;
        mb.callback = callback;
        m_mouseBindings[actionName] = mb;
    }

    void InputManager::BindAxis(const std::string& actionName, GamepadAxis axis,
                                 InputActionCallback callback, float deadzone)
    {
        AxisBinding ab;
        ab.axis = axis;
        ab.deadzone = deadzone;
        ab.callback = callback;
        m_axisBindings[actionName] = ab;
    }

    void InputManager::SetMouseVisible(bool visible)
    {
        SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
    }

    void InputManager::SetMouseRelativeMode(bool enabled)
    {
        SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    }

    void InputManager::SetMouseCapture(bool capture)
    {
        SDL_CaptureMouse(capture ? SDL_TRUE : SDL_FALSE);
    }

} // namespace USE