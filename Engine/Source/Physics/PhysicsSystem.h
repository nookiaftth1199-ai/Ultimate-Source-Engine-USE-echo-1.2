// ============================================================
// Ultimate Source Engine - Physics System
// ============================================================
//
// High-level physics system that abstracts the underlying backend
// (Bullet, PhysX, Jolt). Provides a unified interface for
// creating rigid bodies, soft bodies, vehicles, and performing
// raycasts.
// ============================================================

#pragma once

#include "stdafx.h"
#include "IPhysicsSystem.h"

namespace USE {

    class PhysicsSystem : public IPhysicsSystem {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem();

        // Initialize with a specific backend. Returns true on success.
        bool Initialize(PhysicsBackend backend, const PhysicsWorldSettings& settings = PhysicsWorldSettings());

        // IPhysicsSystem interface
        void Shutdown() override;
        void Update(float deltaTime) override;

        uint32_t CreateRigidBody(const RigidBodyDesc& desc, const CollisionShape& shape) override;
        void DestroyRigidBody(uint32_t bodyId) override;

        Matrix4 GetBodyTransform(uint32_t bodyId) const override;
        void SetBodyTransform(uint32_t bodyId, const Matrix4& transform) override;

        void ApplyForce(uint32_t bodyId, const Vector3& force, const Vector3* relPos = nullptr) override;
        void ApplyImpulse(uint32_t bodyId, const Vector3& impulse, const Vector3* relPos = nullptr) override;
        void SetLinearVelocity(uint32_t bodyId, const Vector3& velocity) override;
        void SetAngularVelocity(uint32_t bodyId, const Vector3& velocity) override;

        uint32_t CreateSoftBody(const SoftBodyDesc& desc) override;
        void DestroySoftBody(uint32_t bodyId) override;
        void GetSoftBodyVertices(uint32_t bodyId, std::vector<Vector3>& outVertices) const override;

        uint32_t CreateVehicle(const VehicleDesc& desc, uint32_t chassisBodyId) override;
        void DestroyVehicle(uint32_t vehicleId) override;
        void SetVehicleInput(uint32_t vehicleId, float throttle, float brake, float steer) override;

        RaycastResult Raycast(const Vector3& origin, const Vector3& direction,
                              float maxDistance, uint32_t layerMask = 0xFFFFFFFF) override;

        void DebugDraw() override;

        const char* GetBackendName() const override;

    private:
        IPhysicsSystem* m_impl;   // Pointer to the selected backend implementation
    };

} // namespace USE