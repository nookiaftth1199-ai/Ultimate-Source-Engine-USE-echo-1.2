// ============================================================
// Ultimate Source Engine - Input Binding
// ============================================================
//
// Represents a single binding between an input (key, mouse button,
// gamepad button, gamepad axis) and an action name.
// Used by the InputManager to map player input to game actions.
// ============================================================

#pragma once

#include "stdafx.h"
#include <SDL.h>
#include <string>
#include <functional>

namespace USE {

    // Type of input source
    enum class InputSourceType {
        Keyboard,
        MouseButton,
        GamepadButton,
        GamepadAxis
    };

    // A single input binding
    struct InputBinding {
        InputSourceType type;
        union {
            SDL_Scancode keyCode;          // for Keyboard
            int          mouseButton;       // for MouseButton (SDL_BUTTON_*)
            int          gamepadButton;     // for GamepadButton (SDL_GameControllerButton)
            int          gamepadAxis;       // for GamepadAxis (SDL_GameControllerAxis)
        };
        float deadzone;                      // for axes (0.0–1.0)
        bool  invert;                         // for axes (invert direction)

        InputBinding()
            : type(InputSourceType::Keyboard)
            , keyCode(SDL_SCANCODE_UNKNOWN)
            , deadzone(0.1f)
            , invert(false)
        {}

        // Convenience constructors
        static InputBinding Key(SDL_Scancode key) {
            InputBinding b;
            b.type = InputSourceType::Keyboard;
            b.keyCode = key;
            return b;
        }

        static InputBinding MouseButton(int button) {
            InputBinding b;
            b.type = InputSourceType::MouseButton;
            b.mouseButton = button;
            return b;
        }

        static InputBinding GamepadButton(int button) {
            InputBinding b;
            b.type = InputSourceType::GamepadButton;
            b.gamepadButton = button;
            return b;
        }

        static InputBinding GamepadAxis(int axis, float deadzone = 0.1f, bool invert = false) {
            InputBinding b;
            b.type = InputSourceType::GamepadAxis;
            b.gamepadAxis = axis;
            b.deadzone = deadzone;
            b.invert = invert;
            return b;
        }
    };

} // namespace USE