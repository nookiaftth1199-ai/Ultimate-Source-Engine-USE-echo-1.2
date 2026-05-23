// ============================================================
// Ultimate Source Engine - Particle
//============================================================
//
// Represents a single particle in a particle system.
// Contains position, velocity, color, size, and lifetime.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace USE {

    class Particle {
    public:
        Particle();
        ~Particle() = default;

        // Initialize the particle with given parameters
        void Initialize(const Vector3& position,
                        const Vector3& velocity,
                        const Color& color,
                        float size,
                        float lifetime);

        // Update the particle (move, age, interpolate color/size)
        // Returns true if still alive, false if lifetime ended.
        bool Update(float deltaTime);

        // Getters
        const Vector3& GetPosition() const { return m_position; }
        const Vector3& GetVelocity() const { return m_velocity; }
        const Color&   GetColor() const   { return m_color; }
        float          GetSize() const    { return m_size; }
        float          GetAge() const     { return m_age; }
        float          GetLifetime() const { return m_lifetime; }
        bool           IsAlive() const    { return m_alive; }

        // Setters for runtime modification
        void SetPosition(const Vector3& pos) { m_position = pos; }
        void SetVelocity(const Vector3& vel) { m_velocity = vel; }
        void SetColor(const Color& col)      { m_color = col; }
        void SetSize(float size)             { m_size = size; }

    private:
        Vector3 m_position;
        Vector3 m_velocity;
        Color   m_color;
        float   m_size;
        float   m_age;
        float   m_lifetime;
        bool    m_alive;
    };

} // namespace USE