// ============================================================
// Ultimate Source Engine - Physics Component
// ============================================================
//
// Component that attaches a physics body to an entity.
// It holds a RigidBody instance and synchronizes the entity's
// transform with the physics world.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include "Physics/RigidBody.h"
#include "Physics/CollisionShape.h"
#include "Physics/PhysicsMaterial.h"

namespace USE {

    // Physics body type (corresponds to RigidBodyType)
    enum class PhysicsBodyType {
        Static,
        Dynamic,
        Kinematic
    };

    class PhysicsComponent : public Component {
    public:
        PhysicsComponent();
        virtual ~PhysicsComponent();

        // Initialize the physics body. Must be called after the entity
        // is placed in the world (requires a PhysicsWorld pointer).
        bool Initialize(PhysicsWorld* world,
                        PhysicsBodyType type,
                        const CollisionShape& shape,
                        float mass = 0.0f,
                        const PhysicsMaterial& material = PhysicsMaterial());

        // Update the component – called every frame. If kinematic,
        // the entity's transform is copied to the body.
        void Update(float deltaTime) override;

        // Late update – if dynamic, the body's transform is copied back
        // to the entity (after physics simulation).
        void LateUpdate(float deltaTime) override;

        // Apply force/impulse (only valid for dynamic bodies)
        void ApplyForce(const Vector3& force, const Vector3* relPos = nullptr);
        void ApplyImpulse(const Vector3& impulse, const Vector3* relPos = nullptr);

        // Access to the underlying rigid body (if needed)
        RigidBody* GetRigidBody() { return m_rigidBody; }

        // Check if the component has a valid physics body.
        bool IsValid() const { return m_rigidBody != nullptr; }

        // Type name
        virtual const char* GetTypeName() const override { return "PhysicsComponent"; }

    private:
        RigidBody*       m_rigidBody;
        PhysicsWorld*    m_world;
        PhysicsBodyType  m_bodyType;
        bool             m_initialized;
    };

} // namespace USE