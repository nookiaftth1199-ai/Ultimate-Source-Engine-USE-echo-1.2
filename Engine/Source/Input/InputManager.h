// ============================================================
// Ultimate Source Engine - Input Manager
//============================================================
//
// Handles keyboard, mouse, and gamepad input.
// Provides current and previous frame state, delta mouse movement,
// and an action binding system.
// ============================================================

#pragma once

#include "stdafx.h"
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <functional>

namespace USE {

    // Mouse button enum (matches SDL)
    enum MouseButton {
        MouseButton_Left = SDL_BUTTON_LEFT,
        MouseButton_Middle = SDL_BUTTON_MIDDLE,
        MouseButton_Right = SDL_BUTTON_RIGHT,
        MouseButton_X1 = SDL_BUTTON_X1,
        MouseButton_X2 = SDL_BUTTON_X2
    };

    // Key state flags
    enum KeyState {
        KeyState_Up = 0,
        KeyState_Down = 1,
        KeyState_Pressed = 2,   // just pressed this frame
        KeyState_Released = 4    // just released this frame
    };

    // Gamepad axis indices
    enum GamepadAxis {
        GamepadAxis_LeftX,
        GamepadAxis_LeftY,
        GamepadAxis_RightX,
        GamepadAxis_RightY,
        GamepadAxis_LeftTrigger,
        GamepadAxis_RightTrigger,
        GamepadAxis_Count
    };

    // Input action callback
    using InputActionCallback = std::function<void(float value)>;

    class InputManager {
    public:
        InputManager();
        ~InputManager();

        // Initialize SDL input subsystems
        bool Initialize();
        void Shutdown();

        // Must be called once per frame to update input state
        void Update();

        // Keyboard queries
        bool IsKeyDown(SDL_Scancode key) const;
        bool IsKeyPressed(SDL_Scancode key) const;  // just pressed this frame
        bool IsKeyReleased(SDL_Scancode key) const; // just released this frame

        // Mouse queries
        bool IsMouseButtonDown(int button) const;
        bool IsMouseButtonPressed(int button) const;
        bool IsMouseButtonReleased(int button) const;

        void GetMousePosition(int& x, int& y) const;
        void GetMouseDelta(int& dx, int& dy) const;
        int GetMouseWheelDelta() const { return m_mouseWheelDelta; }

        // Gamepad queries (multiple controllers supported)
        bool IsGamepadConnected(int index = 0) const;
        float GetGamepadAxis(int index, GamepadAxis axis) const; // returns -1..1
        bool IsGamepadButtonDown(int index, int button) const;   // SDL gamepad button enum

        // Action binding
        void BindAction(const std::string& actionName, SDL_Scancode key,
                        InputActionCallback callback);
        void BindAction(const std::string& actionName, int mouseButton,
                        InputActionCallback callback);
        void BindAxis(const std::string& actionName, GamepadAxis axis,
                      InputActionCallback callback, float deadzone = 0.1f);

        // Set mouse visible/captured/relative mode
        void SetMouseVisible(bool visible);
        void SetMouseRelativeMode(bool enabled);
        void SetMouseCapture(bool capture);

    private:
        // Keyboard state
        const Uint8* m_keyboardState;
        Uint8*       m_previousKeyboardState; // owned copy
        int          m_keyboardStateLength;

        // Mouse state
        Uint32       m_mouseState;
        Uint32       m_previousMouseState;
        int          m_mouseX, m_mouseY;
        int          m_mouseDeltaX, m_mouseDeltaY;
        int          m_mouseWheelDelta;

        // Gamepad state
        static const int MAX_GAMEPADS = 4;
        SDL_GameController* m_gamepads[MAX_GAMEPADS];
        float               m_axisState[MAX_GAMEPADS][GamepadAxis_Count];
        Uint8               m_buttonState[MAX_GAMEPADS][SDL_CONTROLLER_BUTTON_MAX];
        Uint8               m_previousButtonState[MAX_GAMEPADS][SDL_CONTROLLER_BUTTON_MAX];

        // Action bindings
        struct KeyBinding {
            SDL_Scancode key;
            InputActionCallback callback;
        };
        struct MouseBinding {
            int button;
            InputActionCallback callback;
        };
        struct AxisBinding {
            GamepadAxis axis;
            float deadzone;
            InputActionCallback callback;
        };

        std::unordered_map<std::string, KeyBinding>   m_keyBindings;
        std::unordered_map<std::string, MouseBinding> m_mouseBindings;
        std::unordered_map<std::string, AxisBinding>  m_axisBindings;

        // Helper: update gamepad state
        void UpdateGamepads();
    };

} // namespace USE