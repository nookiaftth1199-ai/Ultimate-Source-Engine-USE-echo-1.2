// ============================================================
// Ultimate Source Engine - Gamepad Device Implementation
// ============================================================

#include "stdafx.h"
#include "GamepadDevice.h"
#include "Core/Logger.h"

namespace USE {

    GamepadDevice::GamepadDevice(int deviceIndex)
        : m_controller(nullptr)
        , m_deviceIndex(deviceIndex)
        , m_instanceId(-1)
    {
        m_axisValues.fill(0.0f);
        m_currentButtons.fill(0);
        m_previousButtons.fill(0);
    }

    GamepadDevice::~GamepadDevice()
    {
        if (m_controller) {
            SDL_GameControllerClose(m_controller);
            m_controller = nullptr;
        }
    }

    bool GamepadDevice::Initialize()
    {
        if (!SDL_IsGameController(m_deviceIndex)) {
            USE_LOG_ERROR("GamepadDevice: Device %d is not a game controller", m_deviceIndex);
            return false;
        }

        m_controller = SDL_GameControllerOpen(m_deviceIndex);
        if (!m_controller) {
            USE_LOG_ERROR("GamepadDevice: Failed to open game controller %d: %s",
                          m_deviceIndex, SDL_GetError());
            return false;
        }

        SDL_Joystick* joystick = SDL_GameControllerGetJoystick(m_controller);
        m_instanceId = SDL_JoystickInstanceID(joystick);

        const char* name = SDL_GameControllerName(m_controller);
        m_name = name ? name : "Unknown Gamepad";

        USE_LOG_INFO("GamepadDevice: Opened '%s' (index %d, instance %d)",
                     m_name.c_str(), m_deviceIndex, m_instanceId);
        return true;
    }

    void GamepadDevice::Update()
    {
        if (!m_controller) return;

        // Update axes
        for (int i = 0; i < static_cast<int>(GamepadAxis::Count); ++i) {
            Sint16 raw = SDL_GameControllerGetAxis(m_controller,
                static_cast<SDL_GameControllerAxis>(i));

            // Normalize to -1..1
            float norm = raw / 32767.0f;
            if (norm < -1.0f) norm = -1.0f; // clamp just in case

            // For triggers, 0..1 range is more natural
            if (i == static_cast<int>(GamepadAxis::LeftTrigger) ||
                i == static_cast<int>(GamepadAxis::RightTrigger)) {
                norm = (raw + 32768) / 65535.0f; // maps -32768..32767 to 0..1
            }

            m_axisValues[i] = norm;
        }

        // Update buttons
        m_previousButtons = m_currentButtons;
        for (int i = 0; i < static_cast<int>(GamepadButton::Count); ++i) {
            Uint8 state = SDL_GameControllerGetButton(m_controller,
                static_cast<SDL_GameControllerButton>(i));
            m_currentButtons[i] = state;
        }
    }

    float GamepadDevice::GetAxis(GamepadAxis axis) const
    {
        int idx = static_cast<int>(axis);
        if (idx >= 0 && idx < static_cast<int>(GamepadAxis::Count))
            return m_axisValues[idx];
        return 0.0f;
    }

    float GamepadDevice::GetAxisRaw(GamepadAxis axis) const
    {
        if (!m_controller) return 0.0f;
        int idx = static_cast<int>(axis);
        if (idx < 0 || idx >= static_cast<int>(GamepadAxis::Count))
            return 0.0f;
        return SDL_GameControllerGetAxis(m_controller,
            static_cast<SDL_GameControllerAxis>(idx));
    }

    bool GamepadDevice::IsButtonDown(GamepadButton button) const
    {
        int idx = static_cast<int>(button);
        if (idx >= 0 && idx < static_cast<int>(GamepadButton::Count))
            return m_currentButtons[idx] != 0;
        return false;
    }

    bool GamepadDevice::IsButtonPressed(GamepadButton button) const
    {
        int idx = static_cast<int>(button);
        if (idx >= 0 && idx < static_cast<int>(GamepadButton::Count))
            return m_currentButtons[idx] != 0 && m_previousButtons[idx] == 0;
        return false;
    }

    bool GamepadDevice::IsButtonReleased(GamepadButton button) const
    {
        int idx = static_cast<int>(button);
        if (idx >= 0 && idx < static_cast<int>(GamepadButton::Count))
            return m_currentButtons[idx] == 0 && m_previousButtons[idx] != 0;
        return false;
    }

    bool GamepadDevice::SetRumble(float lowFrequency, float highFrequency, Uint32 durationMs)
    {
        if (!m_controller) return false;
        return SDL_GameControllerRumble(m_controller,
            static_cast<Uint16>(lowFrequency * 0xFFFF),
            static_cast<Uint16>(highFrequency * 0xFFFF),
            durationMs) == 0;
    }

} // namespace USE