// ============================================================
// Ultimate Source Engine - Physics Joint
//============================================================
//
// Represents a constraint between two rigid bodies.
// Supports various joint types: fixed, hinge, ball, slider, etc.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include <cstdint>

namespace USE {

    // Forward declarations
    class PhysicsWorld;

    // Joint types supported by the engine
    enum class JointType {
        Fixed,
        Hinge,
        Ball,
        Slider,
        ConeTwist,
        Generic6DOF
    };

    // Base descriptor for all joints
    struct JointDesc {
        JointType type;
        uint32_t bodyA;
        uint32_t bodyB;
        Matrix4 frameA;      // local transform relative to body A
        Matrix4 frameB;      // local transform relative to body B
        bool collideConnected = false;

        JointDesc() : type(JointType::Fixed), bodyA(0), bodyB(0), collideConnected(false) {}
    };

    // Hinge joint descriptor (rotational constraint around one axis)
    struct HingeJointDesc : public JointDesc {
        bool useLimit = false;
        float lowerLimit = 0.0f;      // radians
        float upperLimit = 0.0f;      // radians
        bool useMotor = false;
        float motorTargetVelocity = 0.0f; // radians/sec
        float maxMotorImpulse = FLT_MAX;

        HingeJointDesc() { type = JointType::Hinge; }
    };

    // Ball (point‑to‑point) joint descriptor
    struct BallJointDesc : public JointDesc {
        // Optional swing cone limit
        float coneLimit = FLT_MAX;     // radians (max angle from rest)

        BallJointDesc() { type = JointType::Ball; }
    };

    // Slider (prismatic) joint descriptor
    struct SliderJointDesc : public JointDesc {
        bool useLimit = false;
        float lowerLimit = 0.0f;       // world units
        float upperLimit = 0.0f;       // world units
        bool useMotor = false;
        float motorTargetVelocity = 0.0f; // world units/sec
        float maxMotorForce = FLT_MAX;

        SliderJointDesc() { type = JointType::Slider; }
    };

    // Fixed joint descriptor (rigidly connects two bodies)
    struct FixedJointDesc : public JointDesc {
        FixedJointDesc() { type = JointType::Fixed; }
    };

    // PhysicsJoint – a handle to a joint in the physics world
    class PhysicsJoint {
    public:
        PhysicsJoint();
        ~PhysicsJoint();

        // Not copyable (joints are unique resources)
        PhysicsJoint(const PhysicsJoint&) = delete;
        PhysicsJoint& operator=(const PhysicsJoint&) = delete;

        // Moveable
        PhysicsJoint(PhysicsJoint&& other) noexcept;
        PhysicsJoint& operator=(PhysicsJoint&& other) noexcept;

        // Check if the joint is valid
        bool IsValid() const { return m_jointId != 0; }

        // Get the underlying physics system ID
        uint32_t GetJointId() const { return m_jointId; }

        // Get the joint type
        JointType GetType() const { return m_type; }

    private:
        uint32_t   m_jointId;
        JointType  m_type;

        // Only PhysicsWorld can create joints
        friend class PhysicsWorld;
    };

} // namespace USE