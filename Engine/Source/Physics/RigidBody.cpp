// ============================================================
// Ultimate Source Engine - Rigid Body Implementation
// ============================================================

#include "stdafx.h"
#include "RigidBody.h"
#include "PhysicsWorld.h"
#include "Core/Logger.h"

namespace USE {

    RigidBody::RigidBody()
        : m_world(nullptr)
        , m_bodyId(0)
        , m_type(RigidBodyType::Static)
    {
    }

    RigidBody::~RigidBody()
    {
        Destroy();
    }

    bool RigidBody::Initialize(PhysicsWorld* world,
                               RigidBodyType type,
                               const Vector3& halfExtents,
                               float mass)
    {
        if (!world) {
            USE_LOG_ERROR("RigidBody::Initialize: world is null");
            return false;
        }

        m_world = world;
        m_type = type;

        float actualMass = (type == RigidBodyType::Dynamic) ? mass : 0.0f;

        m_bodyId = world->CreateBoxBody(halfExtents,
                                        Matrix4::Identity(),
                                        actualMass,
                                        0); // entityId = 0 means not associated with an entity yet

        if (m_bodyId == 0) {
            USE_LOG_ERROR("RigidBody::Initialize: failed to create box body");
            return false;
        }

        return true;
    }

    bool RigidBody::Initialize(PhysicsWorld* world,
                               RigidBodyType type,
                               float radius,
                               float mass)
    {
        if (!world) {
            USE_LOG_ERROR("RigidBody::Initialize: world is null");
            return false;
        }

        m_world = world;
        m_type = type;

        float actualMass = (type == RigidBodyType::Dynamic) ? mass : 0.0f;

        m_bodyId = world->CreateSphereBody(radius,
                                           Matrix4::Identity(),
                                           actualMass,
                                           0);

        if (m_bodyId == 0) {
            USE_LOG_ERROR("RigidBody::Initialize: failed to create sphere body");
            return false;
        }

        return true;
    }

    bool RigidBody::Initialize(PhysicsWorld* world,
                               RigidBodyType type,
                               float radius,
                               float height,
                               float mass)
    {
        if (!world) {
            USE_LOG_ERROR("RigidBody::Initialize: world is null");
            return false;
        }

        m_world = world;
        m_type = type;

        float actualMass = (type == RigidBodyType::Dynamic) ? mass : 0.0f;

        m_bodyId = world->CreateCapsuleBody(radius,
                                            height,
                                            Matrix4::Identity(),
                                            actualMass,
                                            0);

        if (m_bodyId == 0) {
            USE_LOG_ERROR("RigidBody::Initialize: failed to create capsule body");
            return false;
        }

        return true;
    }

    void RigidBody::Destroy()
    {
        if (m_world && m_bodyId != 0) {
            m_world->DestroyBody(m_bodyId);
            m_bodyId = 0;
        }
    }

    Matrix4 RigidBody::GetTransform() const
    {
        if (!m_world || m_bodyId == 0) return Matrix4::Identity();
        return m_world->GetBodyTransform(m_bodyId);
    }

    void RigidBody::SetTransform(const Matrix4& transform)
    {
        if (m_world && m_bodyId != 0) {
            m_world->SetBodyTransform(m_bodyId, transform);
        }
    }

    Vector3 RigidBody::GetLinearVelocity() const
    {
        if (!m_world || m_bodyId == 0) return Vector3::Zero;
        return m_world->GetBodyLinearVelocity(m_bodyId);
    }

    void RigidBody::SetLinearVelocity(const Vector3& velocity)
    {
        if (m_world && m_bodyId != 0) {
            m_world->SetBodyLinearVelocity(m_bodyId, velocity);
        }
    }

    Vector3 RigidBody::GetAngularVelocity() const
    {
        if (!m_world || m_bodyId == 0) return Vector3::Zero;
        return m_world->GetBodyAngularVelocity(m_bodyId);
    }

    void RigidBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (m_world && m_bodyId != 0) {
            m_world->SetBodyAngularVelocity(m_bodyId, velocity);
        }
    }

    void RigidBody::ApplyForce(const Vector3& force, const Vector3* relPos)
    {
        if (m_world && m_bodyId != 0) {
            m_world->ApplyForce(m_bodyId, force, relPos);
        }
    }

    void RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3* relPos)
    {
        if (m_world && m_bodyId != 0) {
            m_world->ApplyImpulse(m_bodyId, impulse, relPos);
        }
    }

} // namespace USE