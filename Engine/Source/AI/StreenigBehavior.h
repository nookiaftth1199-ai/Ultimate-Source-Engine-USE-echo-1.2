// ============================================================
// Ultimate Source Engine - Steering Behaviors
//============================================================
//
// Collection of steering behaviors for AI agents. Each behavior
// computes a desired velocity based on the agent's state and the
// behavior's parameters.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"

namespace USE {

    // Information about the agent (passed to each behavior)
    struct SteeringAgentInfo {
        Vector3 position;
        Vector3 velocity;
        float   maxSpeed;
        float   maxForce;
        float   mass;
    };

    // Base class for all steering behaviors
    class SteeringBehavior {
    public:
        virtual ~SteeringBehavior() = default;
        virtual Vector3 Calculate(const SteeringAgentInfo& agent) = 0;
    };

    // Seek behavior: move towards a target
    class SeekBehavior : public SteeringBehavior {
    public:
        explicit SeekBehavior(const Vector3& target);
        void SetTarget(const Vector3& target) { m_target = target; }
        Vector3 Calculate(const SteeringAgentInfo& agent) override;

    private:
        Vector3 m_target;
    };

    // Flee behavior: move away from a target
    class FleeBehavior : public SteeringBehavior {
    public:
        explicit FleeBehavior(const Vector3& target);
        void SetTarget(const Vector3& target) { m_target = target; }
        Vector3 Calculate(const SteeringAgentInfo& agent) override;

    private:
        Vector3 m_target;
    };

    // Arrive behavior: slow down as it approaches target
    class ArriveBehavior : public SteeringBehavior {
    public:
        ArriveBehavior(const Vector3& target, float slowingDistance = 1.0f);
        void SetTarget(const Vector3& target) { m_target = target; }
        void SetSlowingDistance(float dist) { m_slowingDistance = dist; }
        Vector3 Calculate(const SteeringAgentInfo& agent) override;

    private:
        Vector3 m_target;
        float   m_slowingDistance;
    };

    // Wander behavior: random walk
    class WanderBehavior : public SteeringBehavior {
    public:
        WanderBehavior(float wanderRadius = 1.0f, float wanderDistance = 2.0f,
                       float wanderJitter = 0.1f);
        Vector3 Calculate(const SteeringAgentInfo& agent) override;

    private:
        float m_wanderRadius;
        float m_wanderDistance;
        float m_wanderJitter;
        Vector3 m_wanderTarget; // current wander target on the circle
        void RandomizeWanderTarget();
    };

    // Align behavior: match the orientation of a target (for rotation)
    // Not implemented here.

} // namespace USE