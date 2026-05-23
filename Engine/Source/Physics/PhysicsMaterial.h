// ============================================================
// Ultimate Source Engine - Physics Material
//============================================================
//
// Defines physical properties of surfaces: friction, restitution, density.
// Used by rigid bodies and collision shapes.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class PhysicsMaterial {
    public:
        // Constructor with default values
        PhysicsMaterial(const std::string& name = "Default");

        // Properties
        void SetStaticFriction(float friction) { m_staticFriction = friction; }
        float GetStaticFriction() const { return m_staticFriction; }

        void SetDynamicFriction(float friction) { m_dynamicFriction = friction; }
        float GetDynamicFriction() const { return m_dynamicFriction; }

        void SetRestitution(float restitution) { m_restitution = restitution; }
        float GetRestitution() const { return m_restitution; }

        void SetDensity(float density) { m_density = density; }
        float GetDensity() const { return m_density; }

        // Name
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        // Combine two materials (for collision response). Static utility.
        static float CombineFriction(float frictionA, float frictionB);
        static float CombineRestitution(float restitutionA, float restitutionB);

    private:
        std::string m_name;
        float m_staticFriction;
        float m_dynamicFriction;
        float m_restitution;
        float m_density;
    };

} // namespace USE