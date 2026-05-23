// ============================================================
// Ultimate Source Engine - Character Controller Implementation
// ============================================================

#include "stdafx.h"
#include "CharacterController.h"
#include "Core/Logger.h"

namespace USE {

    CharacterController::CharacterController()
        : m_world(nullptr)
        , m_body(nullptr)
        , m_bodyId(0)
        , m_onGround(false)
        , m_radius(0.3f)
        , m_height(1.8f)
        , m_mass(80.0f)
    {
    }

    CharacterController::~CharacterController()
    {
        Shutdown();
    }

    bool CharacterController::Initialize(PhysicsWorld* world,
                                          float radius,
                                          float height,
                                          float mass,
                                          const Vector3& initialPosition)
    {
        if (!world) {
            USE_LOG_ERROR("CharacterController: world is null");
            return false;
        }

        m_world = world;
        m_radius = radius;
        m_height = height;
        m_mass = mass;

        // Create a rigid body for the character.
        // We'll use a kinematic body (mass = 0) because we control movement manually.
        // Some engines use dynamic bodies with kinematic behavior; here we use kinematic.
        // We'll create a capsule body.
        m_bodyId = m_world->CreateCapsuleBody(radius, height,
                                              Matrix4::Translation(initialPosition),
                                              0.0f, // mass = 0 for kinematic
                                              0);   // no entity id

        if (m_bodyId == 0) {
            USE_LOG_ERROR("CharacterController: Failed to create capsule body");
            return false;
        }

        // Optionally, we could store a RigidBody object, but we only need the ID.
        // For convenience, we'll create a RigidBody instance that wraps the ID.
        m_body = new RigidBody();
        // We need to set its internal bodyId. Since RigidBody doesn't have a setter,
        // we'll just use the ID directly for physics operations.
        // We can still use m_body for some operations, but we'll mostly use m_world.

        // For simplicity, we'll just store the ID and use world functions.

        return true;
    }

    void CharacterController::Shutdown()
    {
        if (m_world && m_bodyId != 0) {
            m_world->DestroyBody(m_bodyId);
            m_bodyId = 0;
        }
        delete m_body;
        m_body = nullptr;
    }

    void CharacterController::SetPosition(const Vector3& pos)
    {
        if (m_world && m_bodyId != 0) {
            Matrix4 transform = Matrix4::Translation(pos);
            m_world->SetBodyTransform(m_bodyId, transform);
        }
    }

    Vector3 CharacterController::GetPosition() const
    {
        if (m_world && m_bodyId != 0) {
            Matrix4 transform = m_world->GetBodyTransform(m_bodyId);
            return Vector3(transform.m[3][0], transform.m[3][1], transform.m[3][2]);
        }
        return Vector3::Zero;
    }

    void CharacterController::SetVelocity(const Vector3& velocity)
    {
        m_velocity = velocity;
    }

    void CharacterController::Jump(float strength)
    {
        if (m_onGround) {
            m_velocity.y += strength;
        }
    }

    void CharacterController::Update(float deltaTime)
    {
        if (!m_world || m_bodyId == 0) return;

        // Apply gravity
        Vector3 gravity(0, -9.81f, 0);
        m_velocity += gravity * deltaTime;

        // Simple ground check: raycast downwards
        Vector3 origin = GetPosition();
        float capsuleBottomOffset = m_height * 0.5f; // assuming capsule origin at center
        RaycastResult hit = m_world->Raycast(origin, Vector3(0, -1, 0), capsuleBottomOffset + 0.1f);
        m_onGround = hit.hit && hit.normal.y > 0.7f; // near vertical

        if (m_onGround && m_velocity.y < 0) {
            m_velocity.y = 0; // stop downward velocity when grounded
        }

        // Move the character using the desired velocity
        MoveAndSlide(m_velocity * deltaTime, deltaTime);
    }

    void CharacterController::MoveAndSlide(const Vector3& desiredVelocity, float deltaTime)
    {
        // This is a simplified movement: just set the position directly.
        // A real implementation would perform collision detection and slide along walls.
        // For now, we just update the position.
        Vector3 newPos = GetPosition() + desiredVelocity;
        SetPosition(newPos);

        // In a complete implementation, you would:
        // 1. Sweep the capsule from current position to newPos.
        // 2. Find the first collision.
        // 3. Adjust the position and velocity to slide.
        // 4. Repeat up to a few iterations.
    }

} // namespace USE