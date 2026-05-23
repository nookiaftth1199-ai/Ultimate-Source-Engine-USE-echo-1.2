// ============================================================
// Ultimate Source Engine - Particle Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Particle.h"
#include "Math/MathUtils.h"

namespace USE {

    Particle::Particle()
        : m_position(0,0,0)
        , m_velocity(0,0,0)
        , m_color(1,1,1,1)
        , m_size(1.0f)
        , m_age(0.0f)
        , m_lifetime(0.0f)
        , m_alive(false)
    {
    }

    void Particle::Initialize(const Vector3& position,
                              const Vector3& velocity,
                              const Color& color,
                              float size,
                              float lifetime)
    {
        m_position = position;
        m_velocity = velocity;
        m_color = color;
        m_size = size;
        m_age = 0.0f;
        m_lifetime = lifetime;
        m_alive = true;
    }

    bool Particle::Update(float deltaTime)
    {
        if (!m_alive) return false;

        // Advance age
        m_age += deltaTime;
        if (m_age >= m_lifetime) {
            m_alive = false;
            return false;
        }

        // Update position based on velocity
        m_position += m_velocity * deltaTime;

        return true;
    }

} // namespace USE