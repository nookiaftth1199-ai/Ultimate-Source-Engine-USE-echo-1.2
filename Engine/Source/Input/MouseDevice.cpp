// ============================================================
// Ultimate Source Engine - Mouse Device Implementation
// ============================================================

#include "stdafx.h"
#include "MouseDevice.h"
#include <cstring>

namespace USE {

    MouseDevice::MouseDevice()
        : m_currentState(0)
        , m_previousState(0)
        , m_x(0)
        , m_y(0)
        , m_deltaX(0)
        , m_deltaY(0)
        , m_wheelDelta(0)
    {
    }

    MouseDevice::~MouseDevice()
    {
    }

    bool MouseDevice::Initialize()
    {
        // No special initialization needed
        return true;
    }

    void MouseDevice::Update()
    {
        // Save previous button state
        m_previousState = m_currentState;

        // Get current mouse state (position and buttons)
        m_currentState = SDL_GetMouseState(&m_x, &m_y);

        // Get relative motion (delta)
        SDL_GetRelativeMouseState(&m_deltaX, &m_deltaY);

        // Note: wheel delta is set via events in PumpEvents; we need to reset it each frame.
        // We'll assume the application accumulates wheel events into m_wheelDelta, then clears it.
        // This is usually done in the event loop. We'll just provide a method to get it.
    }

    bool MouseDevice::IsButtonDown(int button) const
    {
        return (m_currentState & SDL_BUTTON(button)) != 0;
    }

    bool MouseDevice::IsButtonPressed(int button) const
    {
        Uint32 mask = SDL_BUTTON(button);
        return (m_currentState & mask) != 0 && (m_previousState & mask) == 0;
    }

    bool MouseDevice::IsButtonReleased(int button) const
    {
        Uint32 mask = SDL_BUTTON(button);
        return (m_currentState & mask) == 0 && (m_previousState & mask) != 0;
    }

    void MouseDevice::GetPosition(int& x, int& y) const
    {
        x = m_x;
        y = m_y;
    }

    void MouseDevice::GetDelta(int& dx, int& dy) const
    {
        dx = m_deltaX;
        dy = m_deltaY;
    }

} // namespace USE