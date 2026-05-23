// ============================================================
// Ultimate Source Engine - Physics Joint Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PhysicsJoint.h"

namespace USE {

    PhysicsJoint::PhysicsJoint()
        : m_jointId(0)
        , m_type(JointType::Fixed)
    {
    }

    PhysicsJoint::~PhysicsJoint()
    {
        // The actual destruction is handled by the PhysicsWorld.
        // The joint ID becomes invalid when the world destroys it.
    }

    PhysicsJoint::PhysicsJoint(PhysicsJoint&& other) noexcept
        : m_jointId(other.m_jointId)
        , m_type(other.m_type)
    {
        other.m_jointId = 0;
        other.m_type = JointType::Fixed;
    }

    PhysicsJoint& PhysicsJoint::operator=(PhysicsJoint&& other) noexcept
    {
        if (this != &other) {
            m_jointId = other.m_jointId;
            m_type = other.m_type;
            other.m_jointId = 0;
            other.m_type = JointType::Fixed;
        }
        return *this;
    }

} // namespace USE