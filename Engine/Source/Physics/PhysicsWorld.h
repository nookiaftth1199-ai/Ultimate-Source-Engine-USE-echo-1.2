// ============================================================
// Ultimate Source Engine - Physics World
// ============================================================
//
// Represents a self‑contained physics simulation world. It owns
// a physics backend (Bullet/PhysX/Jolt) and manages rigid bodies,
// soft bodies, and vehicles. Provides a high‑level interface for
// game code and integrates with the ECS via body‑to‑entity mapping.
// ============================================================

#pragma once

#include "stdafx.h"
#include "IPhysicsSystem.h"
#include <unordered_map>
#include <functional>

namespace USE {

    // Forward declarations
    class Entity;

    // Collision callback signature
    using CollisionCallback = std::function<void(uint32_t bodyA, uint32_t bodyB, const Vector3& point, const Vector3& normal, float impulse)>;

    class PhysicsWorld {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        // Initialize with a specific backend. Returns true on success.
        bool Initialize(PhysicsBackend backend, const PhysicsWorldSettings& settings = PhysicsWorldSettings());

        // Shut down the world and release all resources.
        void Shutdown();

        // Step the simulation forward by deltaTime.
        void Update(float deltaTime);

        // -----------------------------------------------------------------
        // Rigid body creation helpers
        // -----------------------------------------------------------------
        uint32_t CreateBoxBody(const Vector3& halfExtents, const Matrix4& transform,
                               float mass = 0.0f, uint32_t entityId = 0);
        uint32_t CreateSphereBody(float radius, const Matrix4& transform,
                                  float mass = 0.0f, uint32_t entityId = 0);
        uint32_t CreateCapsuleBody(float radius, float height, const Matrix4& transform,
                                   float mass = 0.0f, uint32_t entityId = 0);
        uint32_t CreateMeshBody(const std::vector<Vector3>& vertices,
                                const std::vector<uint32_t>& indices,
                                const Matrix4& transform, bool convex = true,
                                float mass = 0.0f, uint32_t entityId = 0);

        // Remove a body by its ID.
        void DestroyBody(uint32_t bodyId);

        // -----------------------------------------------------------------
        // Body state queries
        // -----------------------------------------------------------------
        Matrix4 GetBodyTransform(uint32_t bodyId) const;
        void SetBodyTransform(uint32_t bodyId, const Matrix4& transform);

        Vector3 GetBodyLinearVelocity(uint32_t bodyId) const;
        void SetBodyLinearVelocity(uint32_t bodyId, const Vector3& velocity);

        Vector3 GetBodyAngularVelocity(uint32_t bodyId) const;
        void SetBodyAngularVelocity(uint32_t bodyId, const Vector3& velocity);

        // -----------------------------------------------------------------
        // Forces and impulses
        // -----------------------------------------------------------------
        void ApplyForce(uint32_t bodyId, const Vector3& force, const Vector3* relPos = nullptr);
        void ApplyImpulse(uint32_t bodyId, const Vector3& impulse, const Vector3* relPos = nullptr);

        // -----------------------------------------------------------------
        // Raycasting
        // -----------------------------------------------------------------
        RaycastResult Raycast(const Vector3& origin, const Vector3& direction,
                              float maxDistance, uint32_t layerMask = 0xFFFFFFFF);

        // -----------------------------------------------------------------
        // Collision callbacks
        // -----------------------------------------------------------------
        void SetCollisionCallback(CollisionCallback callback) { m_collisionCallback = callback; }

        // -----------------------------------------------------------------
        // Debug rendering
        // -----------------------------------------------------------------
        void DebugDraw();

        // -----------------------------------------------------------------
        // Backend information
        // -----------------------------------------------------------------
        PhysicsBackend GetBackend() const;
        const char* GetBackendName() const;

    private:
        IPhysicsSystem* m_impl;                     // The actual physics backend world
        std::unordered_map<uint32_t, uint32_t> m_bodyToEntity; // Map physics body ID -> entity ID
        CollisionCallback m_collisionCallback;      // User‑provided collision callback

        // Helper to create a collision shape description
        CollisionShape MakeBoxShape(const Vector3& halfExtents);
        CollisionShape MakeSphereShape(float radius);
        CollisionShape MakeCapsuleShape(float radius, float height);
        CollisionShape MakeMeshShape(const std::vector<Vector3>& vertices,
                                     const std::vector<uint32_t>& indices,
                                     bool convex);
    };

} // namespace USE