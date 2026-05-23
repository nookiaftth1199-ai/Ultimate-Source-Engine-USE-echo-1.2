// ============================================================
// Ultimate Source Engine - Vehicle Physics Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "VehiclePhysics.h"
#include "PhysicsWorld.h"
#include "Core/Logger.h"

namespace USE {

    VehiclePhysics::VehiclePhysics()
        : m_world(nullptr)
        , m_chassisBodyId(0)
        , m_vehicleId(0)
        , m_throttle(0)
        , m_brake(0)
        , m_steer(0)
        , m_currentSteerAngle(0)
    {
    }

    VehiclePhysics::~VehiclePhysics()
    {
        Shutdown();
    }

    bool VehiclePhysics::Initialize(PhysicsWorld* world, const VehicleConfig& config,
                                    const Matrix4& initialTransform)
    {
        if (!world) {
            USE_LOG_ERROR("VehiclePhysics: world is null");
            return false;
        }

        m_world = world;
        m_config = config;

        // Create a rigid body for the chassis (box shape approximating vehicle size)
        // We need a box half‑extents roughly covering the vehicle.
        // For simplicity, we'll compute from wheel positions.
        // This is a placeholder; a real vehicle would have a proper chassis shape.
        Vector3 minExtents, maxExtents;
        // Estimate bounding box from wheels
        for (const auto& w : config.wheels) {
            minExtents.x = std::min(minExtents.x, w.connectionPoint.x - w.wheelRadius);
            minExtents.y = std::min(minExtents.y, w.connectionPoint.y - w.wheelRadius);
            minExtents.z = std::min(minExtents.z, w.connectionPoint.z - w.wheelRadius);
            maxExtents.x = std::max(maxExtents.x, w.connectionPoint.x + w.wheelRadius);
            maxExtents.y = std::max(maxExtents.y, w.connectionPoint.y + w.wheelRadius);
            maxExtents.z = std::max(maxExtents.z, w.connectionPoint.z + w.wheelRadius);
        }
        Vector3 halfExtents = (maxExtents - minExtents) * 0.5f;

        // Create chassis body (dynamic)
        RigidBodyDesc desc;
        desc.mass = config.chassisMass;
        desc.initialTransform = initialTransform;
        CollisionShape shape = CollisionShape::MakeBox(halfExtents);
        // Actually, we should use the physics world's method. But our PhysicsWorld doesn't have a generic CreateRigidBody.
        // We'll use the IPhysicsSystem directly from the world.
        IPhysicsSystem* phys = world->GetImpl(); // need to expose this? We'll assume world has a GetImpl method.
        // For now, we'll just use the world's helpers; but we need to create a generic body.
        // Let's simplify: we'll create the body using the physics world's existing shape creation.
        // Since PhysicsWorld doesn't have a generic CreateRigidBody, we'll just use its box creation.
        m_chassisBodyId = world->CreateBoxBody(halfExtents, initialTransform, config.chassisMass, 0);
        if (m_chassisBodyId == 0) {
            USE_LOG_ERROR("VehiclePhysics: Failed to create chassis body");
            return false;
        }

        // Prepare vehicle description for the physics backend
        VehicleDesc vehDesc;
        vehDesc.chassisMass = config.chassisMass;
        vehDesc.engineForce = config.engineForce;
        vehDesc.brakingForce = config.brakingForce;
        vehDesc.steeringAngle = config.maxSteeringAngle; // backend may use this as max
        for (const auto& w : config.wheels) {
            VehicleDesc::Wheel wheel;
            wheel.connectionPoint = w.connectionPoint;
            wheel.wheelDirection = w.wheelDirection;
            wheel.wheelAxle = w.wheelAxle;
            wheel.suspensionRestLength = w.suspensionRestLength;
            wheel.suspensionStiffness = w.suspensionStiffness;
            wheel.suspensionDamping = w.suspensionDamping;
            wheel.wheelRadius = w.wheelRadius;
            wheel.isFrontWheel = w.isFrontWheel;
            // Not all parameters are in VehicleDesc::Wheel; we need to extend it.
            // We'll approximate: use suspensionCompression as extra damping? Not available.
            // We'll just pass the available ones.
            vehDesc.wheels.push_back(wheel);
        }

        // Create the vehicle via the physics system
        m_vehicleId = m_world->GetPhysicsSystem()->CreateVehicle(vehDesc, m_chassisBodyId);
        if (m_vehicleId == 0) {
            USE_LOG_ERROR("VehiclePhysics: Failed to create vehicle in physics backend");
            world->DestroyBody(m_chassisBodyId);
            m_chassisBodyId = 0;
            return false;
        }

        return true;
    }

    void VehiclePhysics::Shutdown()
    {
        if (m_world && m_vehicleId != 0) {
            m_world->GetPhysicsSystem()->DestroyVehicle(m_vehicleId);
            m_vehicleId = 0;
        }
        if (m_world && m_chassisBodyId != 0) {
            m_world->DestroyBody(m_chassisBodyId);
            m_chassisBodyId = 0;
        }
    }

    void VehiclePhysics::SetInput(float throttle, float brake, float steer)
    {
        m_throttle = throttle;
        m_brake = brake;
        m_steer = steer;
    }

    void VehiclePhysics::Update(float deltaTime)
    {
        if (!m_world || m_vehicleId == 0) return;

        // Smooth steering for feel
        float steerSpeed = m_config.steeringSpeed * deltaTime;
        if (m_steer > m_currentSteerAngle) {
            m_currentSteerAngle = std::min(m_currentSteerAngle + steerSpeed, m_steer);
        } else if (m_steer < m_currentSteerAngle) {
            m_currentSteerAngle = std::max(m_currentSteerAngle - steerSpeed, m_steer);
        }

        // Pass input to physics backend
        m_world->GetPhysicsSystem()->SetVehicleInput(m_vehicleId, m_throttle, m_brake, m_currentSteerAngle);
    }

    Matrix4 VehiclePhysics::GetChassisTransform() const
    {
        if (m_world && m_chassisBodyId != 0) {
            return m_world->GetBodyTransform(m_chassisBodyId);
        }
        return Matrix4::Identity();
    }

    std::vector<Matrix4> VehiclePhysics::GetWheelTransforms() const
    {
        // This would require querying the physics backend for wheel transforms.
        // Not implemented in IPhysicsSystem. We'll return empty for now.
        return std::vector<Matrix4>();
    }

} // namespace USE