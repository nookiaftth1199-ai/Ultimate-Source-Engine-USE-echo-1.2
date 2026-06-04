// ============================================================
// Ultimate Source Engine - Rigid Body
// ============================================================
//
// Represents a physical rigid body that can be attached to an entity.
// Provides methods to create, manipulate, and query the body's
// state in the physics world.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace USE {

    // Forward declarations
    class PhysicsWorld;

    enum class RigidBodyType {
        Static,      // Zero mass, immovable
        Dynamic,     // Movable, affected by forces
        Kinematic    // Movable, but not affected by forces (user controlled)
    };

    class RigidBody {
    public:
        RigidBody();
        ~RigidBody();

        // Initialize the rigid body with a shape and type.
        // Returns true on success.
        bool Initialize(PhysicsWorld* world,
                        RigidBodyType type,
                        const Vector3& halfExtents,
                        float mass = 0.0f);

        bool Initialize(PhysicsWorld* world,
                        RigidBodyType type,
                        float radius,
                        float mass = 0.0f);  // sphere

        bool Initialize(PhysicsWorld* world,
                        RigidBodyType type,
                        float radius,
                        float height,
                        float mass = 0.0f);  // capsule

        // Destroy the rigid body (releases physics resources).
        void Destroy();

        // Get/set the world transform of the body.
        Matrix4 GetTransform() const;
        void    SetTransform(const Matrix4& transform);

        // Velocity access.
        Vector3 GetLinearVelocity() const;
        void    SetLinearVelocity(const Vector3& velocity);
        Vector3 GetAngularVelocity() const;
        void    SetAngularVelocity(const Vector3& velocity);

        // Apply forces and impulses.
        void ApplyForce(const Vector3& force, const Vector3* relPos = nullptr);
        void ApplyImpulse(const Vector3& impulse, const Vector3* relPos = nullptr);

        // Check if the body is valid (has a physics representation).
        bool IsValid() const { return m_bodyId != 0; }

        // Get the underlying physics body ID (for advanced usage).
        uint32_t GetBodyId() const { return m_bodyId; }

        // Get the physics world this body belongs to.
        PhysicsWorld* GetWorld() const { return m_world; }

    private:
        PhysicsWorld* m_world;
        uint32_t      m_bodyId;
        RigidBodyType m_type;
    };

} // namespace USE