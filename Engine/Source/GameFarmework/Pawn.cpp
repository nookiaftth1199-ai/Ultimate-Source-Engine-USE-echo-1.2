// ============================================================
// Ultimate Source Engine - Pawn Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Pawn.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Math/MathUtils.h"

namespace USE {

    Pawn::Pawn()
        : m_transform(nullptr)
        , m_physics(nullptr)
        , m_health(100.0f)
        , m_maxHealth(100.0f)
        , m_moveSpeed(5.0f)
        , m_yawDelta(0.0f)
        , m_pitchDelta(0.0f)
        , m_jumpRequested(false)
    {
    }

    Pawn::~Pawn()
    {
    }

    void Pawn::Update(float deltaTime)
    {
        // Apply accumulated movement (if any)
        if (m_accumulatedMovement.LengthSq() > 0.001f) {
            // If we have a physics component, apply velocity; otherwise move transform directly.
            if (m_physics && m_physics->IsValid()) {
                // Set linear velocity (for dynamic bodies) or push (for kinematic)
                // Simplified: just add impulse? Not correct.
                // For kinematic, we set transform.
                // For now, just update transform.
                SetPosition(GetPosition() + m_accumulatedMovement * deltaTime);
            } else {
                SetPosition(GetPosition() + m_accumulatedMovement * deltaTime);
            }
            m_accumulatedMovement = Vector3::Zero;
        }

        // Apply rotation
        if (m_yawDelta != 0.0f || m_pitchDelta != 0.0f) {
            if (m_transform) {
                Quaternion rot = m_transform->localTransform.rotation;
                rot = Quaternion::FromAxisAngle(Vector3::Up, MathUtils::Radians(m_yawDelta)) * rot;
                // For pitch, rotate around right axis
                Vector3 right = GetRight();
                rot = Quaternion::FromAxisAngle(right, MathUtils::Radians(m_pitchDelta)) * rot;
                m_transform->localTransform.rotation = rot;
                m_transform->UpdateWorldTransform();
            }
            m_yawDelta = 0.0f;
            m_pitchDelta = 0.0f;
        }

        if (m_jumpRequested) {
            if (m_physics && m_physics->IsValid()) {
                m_physics->ApplyImpulse(Vector3(0, 5.0f, 0), nullptr); // simple upward impulse
            }
            m_jumpRequested = false;
        }
    }

    void Pawn::AddMovement(const Vector3& worldDirection, float speed)
    {
        m_accumulatedMovement += worldDirection * speed;
    }

    void Pawn::Jump()
    {
        m_jumpRequested = true;
    }

    void Pawn::AddYaw(float degrees)
    {
        m_yawDelta += degrees;
    }

    void Pawn::AddPitch(float degrees)
    {
        m_pitchDelta += degrees;
    }

    void Pawn::TakeDamage(float amount, Pawn* instigator)
    {
        if (!IsAlive()) return;
        m_health -= amount;
        if (m_health <= 0.0f) {
            Die();
        }
    }

    void Pawn::Die()
    {
        m_health = 0.0f;
        // Optionally trigger death animation, ragdoll, etc.
    }

    void Pawn::Heal(float amount)
    {
        if (!IsAlive()) return;
        m_health += amount;
        if (m_health > m_maxHealth) m_health = m_maxHealth;
    }

    Vector3 Pawn::GetPosition() const
    {
        if (m_transform) return m_transform->worldTransform.translation;
        return Vector3::Zero;
    }

    void Pawn::SetPosition(const Vector3& pos)
    {
        if (m_transform) {
            m_transform->localTransform.translation = pos;
            m_transform->UpdateWorldTransform();
        }
    }

    Vector3 Pawn::GetForward() const
    {
        if (m_transform) return m_transform->worldTransform.rotation * Vector3(0,0,1);
        return Vector3(0,0,1);
    }

    Vector3 Pawn::GetRight() const
    {
        if (m_transform) return m_transform->worldTransform.rotation * Vector3(1,0,0);
        return Vector3(1,0,0);
    }

    Vector3 Pawn::GetUp() const
    {
        if (m_transform) return m_transform->worldTransform.rotation * Vector3(0,1,0);
        return Vector3(0,1,0);
    }

} // namespace USE