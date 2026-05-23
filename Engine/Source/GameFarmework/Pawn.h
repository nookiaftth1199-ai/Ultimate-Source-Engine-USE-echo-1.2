// ============================================================
// Ultimate Source Engine - Pawn
//============================================================
//
// Base class for all characters/players that can be controlled.
// Provides movement, health, damage, and basic physics interaction.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Entity.h"
#include "Math/Vector3.h"

namespace USE {

    // Forward declarations
    class TransformComponent;
    class PhysicsComponent;

    class Pawn : public Entity {
    public:
        Pawn();
        virtual ~Pawn();

        // Called every frame
        virtual void Update(float deltaTime) override;

        // Movement commands (accumulated over frame)
        virtual void AddMovement(const Vector3& worldDirection, float speed);
        virtual void Jump();
        virtual void AddYaw(float degrees);
        virtual void AddPitch(float degrees);

        // Combat
        virtual void TakeDamage(float amount, Pawn* instigator = nullptr);
        virtual void Die();
        virtual void Heal(float amount);

        // Getters / Setters
        Vector3 GetPosition() const;
        void SetPosition(const Vector3& pos);

        Vector3 GetForward() const;
        Vector3 GetRight() const;
        Vector3 GetUp() const;

        float GetHealth() const { return m_health; }
        void SetHealth(float health) { m_health = health; }
        float GetMaxHealth() const { return m_maxHealth; }
        void SetMaxHealth(float maxHealth) { m_maxHealth = maxHealth; }

        float GetMoveSpeed() const { return m_moveSpeed; }
        void SetMoveSpeed(float speed) { m_moveSpeed = speed; }

        bool IsAlive() const { return m_health > 0.0f; }

    protected:
        // Components
        TransformComponent* m_transform;
        PhysicsComponent*   m_physics;

        // Properties
        float m_health;
        float m_maxHealth;
        float m_moveSpeed;

        // Movement state
        Vector3 m_accumulatedMovement;  // world direction * speed, applied in Update
        float   m_yawDelta;             // degrees this frame
        float   m_pitchDelta;            // degrees this frame
        bool    m_jumpRequested;
    };

} // namespace USE