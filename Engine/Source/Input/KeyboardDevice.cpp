// ============================================================
// Ultimate Source Engine - Keyboard Device Implementation
// ============================================================

#include "stdafx.h"
#include "KeyboardDevice.h"
#include <cstring>

namespace USE {

    KeyboardDevice::KeyboardDevice()
        : m_currentState(nullptr)
        , m_stateLength(0)
    {
    }

    KeyboardDevice::~KeyboardDevice()
    {
    }

    bool KeyboardDevice::Initialize()
    {
        // Get pointer to SDL's internal keyboard state
        m_currentState = SDL_GetKeyboardState(&m_stateLength);
        if (!m_currentState) return false;

        // Allocate and zero previous state buffer
        m_previousState.resize(m_stateLength, 0);
        return true;
    }

    void KeyboardDevice::Update()
    {
        if (!m_currentState) return;

        // Copy current state to previous (SDL updates its state via SDL_PumpEvents)
        std::memcpy(m_previousState.data(), m_currentState, m_stateLength);
    }

    bool KeyboardDevice::IsKeyDown(SDL_Scancode key) const
    {
        if (!m_currentState || key >= m_stateLength) return false;
        return m_currentState[key] != 0;
    }

    bool KeyboardDevice::IsKeyPressed(SDL_Scancode key) const
    {
        if (!m_currentState || key >= m_stateLength) return false;
        return m_currentState[key] != 0 && m_previousState[key] == 0;
    }

    bool KeyboardDevice::IsKeyReleased(SDL_Scancode key) const
    {
        if (!m_currentState || key >= m_stateLength) return false;
        return m_currentState[key] == 0 && m_previousState[key] != 0;
    }

} // namespace USE