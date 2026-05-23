// ============================================================
// Ultimate Source Engine - Steering Behaviors Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "SteeringBehavior.h"
#include "Math/MathUtils.h"
#include <cstdlib>

namespace USE {

    // -----------------------------------------------------------------
    // SeekBehavior
    // -----------------------------------------------------------------
    SeekBehavior::SeekBehavior(const Vector3& target) : m_target(target) {}

    Vector3 SeekBehavior::Calculate(const SteeringAgentInfo& agent) {
        Vector3 desired = m_target - agent.position;
        desired.Normalize();
        desired *= agent.maxSpeed;
        Vector3 steering = desired - agent.velocity;
        // Clamp force
        if (steering.LengthSq() > agent.maxForce * agent.maxForce) {
            steering.Normalize();
            steering *= agent.maxForce;
        }
        return steering;
    }

    // -----------------------------------------------------------------
    // FleeBehavior
    // -----------------------------------------------------------------
    FleeBehavior::FleeBehavior(const Vector3& target) : m_target(target) {}

    Vector3 FleeBehavior::Calculate(const SteeringAgentInfo& agent) {
        Vector3 desired = agent.position - m_target;
        desired.Normalize();
        desired *= agent.maxSpeed;
        Vector3 steering = desired - agent.velocity;
        if (steering.LengthSq() > agent.maxForce * agent.maxForce) {
            steering.Normalize();
            steering *= agent.maxForce;
        }
        return steering;
    }

    // -----------------------------------------------------------------
    // ArriveBehavior
    // -----------------------------------------------------------------
    ArriveBehavior::ArriveBehavior(const Vector3& target, float slowingDistance)
        : m_target(target), m_slowingDistance(slowingDistance) {}

    Vector3 ArriveBehavior::Calculate(const SteeringAgentInfo& agent) {
        Vector3 toTarget = m_target - agent.position;
        float distance = toTarget.Length();
        if (distance < 0.001f) return Vector3::Zero;

        float rampedSpeed = agent.maxSpeed * (distance / m_slowingDistance);
        float limitedSpeed = std::min(rampedSpeed, agent.maxSpeed);
        Vector3 desired = toTarget * (limitedSpeed / distance); // normalized direction times speed
        Vector3 steering = desired - agent.velocity;
        if (steering.LengthSq() > agent.maxForce * agent.maxForce) {
            steering.Normalize();
            steering *= agent.maxForce;
        }
        return steering;
    }

    // -----------------------------------------------------------------
    // WanderBehavior
    // -----------------------------------------------------------------
    WanderBehavior::WanderBehavior(float wanderRadius, float wanderDistance, float wanderJitter)
        : m_wanderRadius(wanderRadius)
        , m_wanderDistance(wanderDistance)
        , m_wanderJitter(wanderJitter)
    {
        // Initialize with a random target on the unit circle
        float angle = RandomFloat() * MathUtils::TAU;
        m_wanderTarget.x = cosf(angle) * wanderRadius;
        m_wanderTarget.y = 0.0f;
        m_wanderTarget.z = sinf(angle) * wanderRadius;
    }

    void WanderBehavior::RandomizeWanderTarget() {
        float angle = (RandomFloat() - 0.5f) * m_wanderJitter;
        // Rotate current target around Y axis (for 2D wander)
        // Simple: just add a random offset and re-project onto circle
        m_wanderTarget.x += RandomFloat() * m_wanderJitter - m_wanderJitter * 0.5f;
        m_wanderTarget.z += RandomFloat() * m_wanderJitter - m_wanderJitter * 0.5f;
        float len = m_wanderTarget.Length();
        if (len > 0.001f) {
            m_wanderTarget = m_wanderTarget * (m_wanderRadius / len);
        }
    }

    Vector3 WanderBehavior::Calculate(const SteeringAgentInfo& agent) {
        // Update wander target on the circle
        RandomizeWanderTarget();

        // Project circle forward in front of the agent
        Vector3 forward = agent.velocity;
        forward.Normalize();
        if (forward.LengthSq() < 0.001f) {
            forward = Vector3(0,0,1); // default forward
        }
        Vector3 center = agent.position + forward * m_wanderDistance;

        // Add wander offset (transform to world space)
        // We need a local coordinate system for the agent: forward, right, up.
        Vector3 right = forward.Cross(Vector3::Up).Normalized();
        Vector3 up = right.Cross(forward).Normalized();

        Vector3 offset = right * m_wanderTarget.x + up * m_wanderTarget.y + forward * m_wanderTarget.z;
        // For 2D wander, we ignore up; let's use only x and z.
        offset = right * m_wanderTarget.x + forward * m_wanderTarget.z;

        Vector3 target = center + offset;

        // Seek towards the wander target
        SeekBehavior seek(target);
        return seek.Calculate(agent);
    }

    // Helper: random float (0..1)
    static float RandomFloat() {
        return (float)rand() / (float)RAND_MAX;
    }

} // namespace USE