// ============================================================
// Ultimate Source Engine - Physics System Implementation
// ============================================================

#include "stdafx.h"
#include "PhysicsSystem.h"
#include "Core/Logger.h"

// Include each backend header only if compiled in.
// We'll conditionally include based on preprocessor defines.
#ifdef USE_BULLET
#include "Bullet/BulletPhysicsSystem.h"
#endif
#ifdef USE_PHYSX
#include "PhysX/PhysXPhysicsSystem.h"
#endif
#ifdef USE_JOLT
#include "Jolt/JoltPhysicsSystem.h"
#endif

namespace USE {

    PhysicsSystem::PhysicsSystem()
        : m_impl(nullptr)
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
        Shutdown();
    }

    bool PhysicsSystem::Initialize(PhysicsBackend backend, const PhysicsWorldSettings& settings)
    {
        // If we already have an implementation, shut it down.
        if (m_impl) {
            m_impl->Shutdown();
            delete m_impl;
            m_impl = nullptr;
        }

        // Create the appropriate backend
        switch (backend) {
#ifdef USE_BULLET
            case PhysicsBackend::Bullet:
                m_impl = new BulletPhysicsSystem();
                break;
#endif
#ifdef USE_PHYSX
            case PhysicsBackend::PhysX:
                m_impl = new PhysXPhysicsSystem();
                break;
#endif
#ifdef USE_JOLT
            case PhysicsBackend::Jolt:
                m_impl = new JoltPhysicsSystem();
                break;
#endif
            default:
                USE_LOG_ERROR("PhysicsSystem: Requested backend not compiled in.");
                return false;
        }

        if (!m_impl->Initialize(settings)) {
            USE_LOG_ERROR("PhysicsSystem: Failed to initialize selected backend.");
            delete m_impl;
            m_impl = nullptr;
            return false;
        }

        USE_LOG_INFO("PhysicsSystem: Initialized with backend '%s'.", GetBackendName());
        return true;
    }

    void PhysicsSystem::Shutdown()
    {
        if (m_impl) {
            m_impl->Shutdown();
            delete m_impl;
            m_impl = nullptr;
        }
    }

    void PhysicsSystem::Update(float deltaTime)
    {
        if (m_impl) m_impl->Update(deltaTime);
    }

    uint32_t PhysicsSystem::CreateRigidBody(const RigidBodyDesc& desc, const CollisionShape& shape)
    {
        return m_impl ? m_impl->CreateRigidBody(desc, shape) : 0;
    }

    void PhysicsSystem::DestroyRigidBody(uint32_t bodyId)
    {
        if (m_impl) m_impl->DestroyRigidBody(bodyId);
    }

    Matrix4 PhysicsSystem::GetBodyTransform(uint32_t bodyId) const
    {
        return m_impl ? m_impl->GetBodyTransform(bodyId) : Matrix4::Identity();
    }

    void PhysicsSystem::SetBodyTransform(uint32_t bodyId, const Matrix4& transform)
    {
        if (m_impl) m_impl->SetBodyTransform(bodyId, transform);
    }

    void PhysicsSystem::ApplyForce(uint32_t bodyId, const Vector3& force, const Vector3* relPos)
    {
        if (m_impl) m_impl->ApplyForce(bodyId, force, relPos);
    }

    void PhysicsSystem::ApplyImpulse(uint32_t bodyId, const Vector3& impulse, const Vector3* relPos)
    {
        if (m_impl) m_impl->ApplyImpulse(bodyId, impulse, relPos);
    }

    void PhysicsSystem::SetLinearVelocity(uint32_t bodyId, const Vector3& velocity)
    {
        if (m_impl) m_impl->SetLinearVelocity(bodyId, velocity);
    }

    void PhysicsSystem::SetAngularVelocity(uint32_t bodyId, const Vector3& velocity)
    {
        if (m_impl) m_impl->SetAngularVelocity(bodyId, velocity);
    }

    uint32_t PhysicsSystem::CreateSoftBody(const SoftBodyDesc& desc)
    {
        return m_impl ? m_impl->CreateSoftBody(desc) : 0;
    }

    void PhysicsSystem::DestroySoftBody(uint32_t bodyId)
    {
        if (m_impl) m_impl->DestroySoftBody(bodyId);
    }

    void PhysicsSystem::GetSoftBodyVertices(uint32_t bodyId, std::vector<Vector3>& outVertices) const
    {
        if (m_impl) m_impl->GetSoftBodyVertices(bodyId, outVertices);
    }

    uint32_t PhysicsSystem::CreateVehicle(const VehicleDesc& desc, uint32_t chassisBodyId)
    {
        return m_impl ? m_impl->CreateVehicle(desc, chassisBodyId) : 0;
    }

    void PhysicsSystem::DestroyVehicle(uint32_t vehicleId)
    {
        if (m_impl) m_impl->DestroyVehicle(vehicleId);
    }

    void PhysicsSystem::SetVehicleInput(uint32_t vehicleId, float throttle, float brake, float steer)
    {
        if (m_impl) m_impl->SetVehicleInput(vehicleId, throttle, brake, steer);
    }

    RaycastResult PhysicsSystem::Raycast(const Vector3& origin, const Vector3& direction,
                                         float maxDistance, uint32_t layerMask)
    {
        return m_impl ? m_impl->Raycast(origin, direction, maxDistance, layerMask) : RaycastResult();
    }

    void PhysicsSystem::DebugDraw()
    {
        if (m_impl) m_impl->DebugDraw();
    }

    const char* PhysicsSystem::GetBackendName() const
    {
        return m_impl ? m_impl->GetBackendName() : "None";
    }

} // namespace USEperforming