// ============================================================
// Ultimate Source Engine - Character Controller
// ============================================================
//
// Provides a high‑level character movement controller that
// handles walking, running, jumping, and collision response.
// Uses a capsule collision shape and interacts with the physics world.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/RigidBody.h"

namespace USE {

    class CharacterController {
    public:
        CharacterController();
        ~CharacterController();

        // Initialize the controller with a physics world and capsule dimensions.
        // The controller will create a kinematic rigid body with a capsule shape.
        bool Initialize(PhysicsWorld* world,
                        float radius,
                        float height,
                        float mass = 80.0f,
                        const Vector3& initialPosition = Vector3::Zero);

        // Destroy the controller and release physics resources.
        void Shutdown();

        // Set/get the position of the character.
        void SetPosition(const Vector3& pos);
        Vector3 GetPosition() const;

        // Movement control (apply a desired velocity for this frame).
        void SetVelocity(const Vector3& velocity);
        Vector3 GetVelocity() const { return m_velocity; }

        // Jump (add an upward impulse).
        void Jump(float strength);

        // Check if the character is on the ground.
        bool IsOnGround() const { return m_onGround; }

        // Get the capsule dimensions.
        float GetRadius() const { return m_radius; }
        float GetHeight() const { return m_height; }

        // Update the character (applies gravity, collision detection, and moves the body).
        // Call this once per frame with the time step.
        void Update(float deltaTime);

    private:
        PhysicsWorld* m_world;
        RigidBody*    m_body;       // Underlying rigid body (kinematic)
        uint32_t      m_bodyId;      // ID of the rigid body in the physics world

        Vector3       m_velocity;
        bool          m_onGround;
        float         m_radius;
        float         m_height;
        float         m_mass;

        // Internal helper to perform collision detection and slide movement.
        void MoveAndSlide(const Vector3& desiredVelocity, float deltaTime);
    };

} // namespace USE