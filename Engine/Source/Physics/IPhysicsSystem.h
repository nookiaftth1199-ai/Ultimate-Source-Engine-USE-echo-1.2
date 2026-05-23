// ============================================================
// Ultimate Source Engine - Physics System Interface
// ============================================================
//
// Abstract interface for physics backends. Supports rigid bodies,
// soft bodies, vehicles, and advanced features.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include <cstdint>
#include <vector>

namespace USE {

    // -----------------------------------------------------------------
    // Physics backend types
    // -----------------------------------------------------------------
    enum class PhysicsBackend {
        Bullet,
        PhysX,
        Jolt,
        None
    };

    // -----------------------------------------------------------------
    // Common physics settings
    // -----------------------------------------------------------------
    struct PhysicsWorldSettings {
        Vector3 gravity = Vector3(0, -9.81f, 0);
        float fixedTimeStep = 1.0f / 60.0f;
        int maxSubSteps = 5;
        bool enableMultithreading = true;
    };

    // -----------------------------------------------------------------
    // Rigid body description
    // -----------------------------------------------------------------
    struct RigidBodyDesc {
        float mass = 1.0f;
        float friction = 0.5f;
        float restitution = 0.2f;
        float linearDamping = 0.0f;
        float angularDamping = 0.0f;
        Matrix4 initialTransform = Matrix4::Identity();
        bool isKinematic = false;
        bool isStatic = false;
        bool enableCCD = false;
        uint32_t collisionLayer = 0;
        uint32_t collisionMask = 0xFFFFFFFF;
    };

    // -----------------------------------------------------------------
    // Collision shape types
    // -----------------------------------------------------------------
    enum class CollisionShapeType {
        Sphere,
        Box,
        Capsule,
        Cylinder,
        ConvexHull,
        TriangleMesh,
        Compound
    };

    struct CollisionShape {
        CollisionShapeType type;
        union {
            struct { float radius; } sphere;
            struct { Vector3 halfExtents; } box;
            struct { float radius; float height; } capsule;
            struct { float radius; float height; } cylinder;
        };
        void* userData = nullptr; // for hull/mesh data
    };

    // -----------------------------------------------------------------
    // Soft body description
    // -----------------------------------------------------------------
    struct SoftBodyDesc {
        float mass = 1.0f;
        float pressure = 0.0f;
        float volumeStiffness = 1.0f;
        float bendingStiffness = 0.1f;
        std::vector<Vector3> vertices;          // initial vertex positions
        std::vector<uint32_t> indices;          // triangle indices
        // ... many more parameters possible
    };

    // -----------------------------------------------------------------
    // Vehicle description (simplified)
    // -----------------------------------------------------------------
    struct VehicleDesc {
        float chassisMass = 800.0f;
        float engineForce = 2000.0f;
        float brakingForce = 1000.0f;
        float steeringAngle = 0.5f; // radians
        struct Wheel {
            Vector3 connectionPoint;  // relative to chassis
            Vector3 wheelDirection;   // usually (0,-1,0)
            Vector3 wheelAxle;        // usually (1,0,0) or (-1,0,0)
            float suspensionRestLength = 0.5f;
            float suspensionStiffness = 20.0f;
            float suspensionDamping = 2.0f;
            float wheelRadius = 0.3f;
            bool isFrontWheel = true;
        };
        std::vector<Wheel> wheels;
    };

    // -----------------------------------------------------------------
    // Raycast result
    // -----------------------------------------------------------------
    struct RaycastResult {
        bool hit = false;
        float fraction = 1.0f;
        Vector3 point;
        Vector3 normal;
        uint32_t bodyId = 0;
        void* userData = nullptr;
    };

    // -----------------------------------------------------------------
    // Physics system interface
    // -----------------------------------------------------------------
    class IPhysicsSystem {
    public:
        virtual ~IPhysicsSystem() = default;

        // World management
        virtual bool Initialize(const PhysicsWorldSettings& settings) = 0;
        virtual void Shutdown() = 0;
        virtual void Update(float deltaTime) = 0;

        // Rigid bodies
        virtual uint32_t CreateRigidBody(const RigidBodyDesc& desc,
                                          const CollisionShape& shape) = 0;
        virtual void DestroyRigidBody(uint32_t bodyId) = 0;

        virtual Matrix4 GetBodyTransform(uint32_t bodyId) const = 0;
        virtual void SetBodyTransform(uint32_t bodyId, const Matrix4& transform) = 0;

        virtual void ApplyForce(uint32_t bodyId, const Vector3& force,
                                const Vector3* relPos = nullptr) = 0;
        virtual void ApplyImpulse(uint32_t bodyId, const Vector3& impulse,
                                  const Vector3* relPos = nullptr) = 0;
        virtual void SetLinearVelocity(uint32_t bodyId, const Vector3& velocity) = 0;
        virtual void SetAngularVelocity(uint32_t bodyId, const Vector3& velocity) = 0;

        // Soft bodies
        virtual uint32_t CreateSoftBody(const SoftBodyDesc& desc) = 0;
        virtual void DestroySoftBody(uint32_t bodyId) = 0;
        virtual void GetSoftBodyVertices(uint32_t bodyId, std::vector<Vector3>& outVertices) const = 0;

        // Vehicles
        virtual uint32_t CreateVehicle(const VehicleDesc& desc, uint32_t chassisBodyId) = 0;
        virtual void DestroyVehicle(uint32_t vehicleId) = 0;
        virtual void SetVehicleInput(uint32_t vehicleId, float throttle, float brake, float steer) = 0;

        // Raycasting
        virtual RaycastResult Raycast(const Vector3& origin,
                                      const Vector3& direction,
                                      float maxDistance,
                                      uint32_t layerMask = 0xFFFFFFFF) = 0;

        // Debug drawing
        virtual void DebugDraw() = 0;

        // Get backend name
        virtual const char* GetBackendName() const = 0;
    };

} // namespace USE