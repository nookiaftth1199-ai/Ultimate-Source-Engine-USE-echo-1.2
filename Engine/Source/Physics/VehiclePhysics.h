// ============================================================
// Ultimate Source Engine - Vehicle Physics
//============================================================
//
// Represents a wheeled vehicle that interacts with the physics world.
// Supports engine, transmission, suspension, and steering.
// Uses the underlying physics backend (Bullet, PhysX, Jolt) via the IPhysicsSystem interface.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Physics/IPhysicsSystem.h"
#include <vector>

namespace USE {

    // Forward declaration
    class PhysicsWorld;

    // Wheel definition
    struct VehicleWheel {
        Vector3 connectionPoint;   // position relative to chassis
        Vector3 wheelDirection;    // typically (0, -1, 0)
        Vector3 wheelAxle;         // typically (1, 0, 0) or (-1, 0, 0)
        float suspensionRestLength;
        float suspensionStiffness;
        float suspensionDamping;
        float suspensionCompression;
        float wheelRadius;
        float wheelWidth;
        bool isFrontWheel;
        bool isPowered;
    };

    // Vehicle configuration
    struct VehicleConfig {
        float chassisMass;
        float engineForce;
        float brakingForce;
        float maxSteeringAngle;     // radians
        float steeringSpeed;        // radians per second
        float wheelFriction;
        std::vector<VehicleWheel> wheels;

        VehicleConfig()
            : chassisMass(800.0f)
            , engineForce(2000.0f)
            , brakingForce(1000.0f)
            , maxSteeringAngle(0.5f)
            , steeringSpeed(2.0f)
            , wheelFriction(0.9f)
        {}
    };

    class VehiclePhysics {
    public:
        VehiclePhysics();
        ~VehiclePhysics();

        // Initialize the vehicle in a physics world.
        // The chassis body is created automatically; you can provide an initial transform.
        bool Initialize(PhysicsWorld* world, const VehicleConfig& config,
                        const Matrix4& initialTransform = Matrix4::Identity());

        // Shut down and release physics resources.
        void Shutdown();

        // Set vehicle input (normalized values).
        void SetInput(float throttle, float brake, float steer);

        // Get the chassis body ID (if needed for low-level access).
        uint32_t GetChassisBodyId() const { return m_chassisBodyId; }

        // Get the vehicle ID from the physics backend.
        uint32_t GetVehicleId() const { return m_vehicleId; }

        // Update vehicle state (should be called each frame).
        void Update(float deltaTime);

        // Get current chassis transform.
        Matrix4 GetChassisTransform() const;

        // Get wheel transforms (world space) for rendering.
        std::vector<Matrix4> GetWheelTransforms() const;

    private:
        PhysicsWorld* m_world;
        VehicleConfig m_config;
        uint32_t      m_chassisBodyId;
        uint32_t      m_vehicleId;

        // Current input state
        float m_throttle;
        float m_brake;
        float m_steer;
        float m_currentSteerAngle; // interpolated for smoothing
    };

} // namespace USE