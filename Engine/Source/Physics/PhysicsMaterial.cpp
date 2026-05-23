// ============================================================
// Ultimate Source Engine - Physics Material Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PhysicsMaterial.h"
#include <algorithm>

namespace USE {

    PhysicsMaterial::PhysicsMaterial(const std::string& name)
        : m_name(name)
        , m_staticFriction(0.5f)
        , m_dynamicFriction(0.5f)
        , m_restitution(0.2f)
        , m_density(1.0f)
    {
    }

    float PhysicsMaterial::CombineFriction(float frictionA, float frictionB)
    {
        // Common methods: multiply, average, max, min, geometric mean.
        // Here we use geometric mean for a balanced result.
        return sqrtf(frictionA * frictionB);
    }

    float PhysicsMaterial::CombineRestitution(float restitutionA, float restitutionB)
    {
        // Typically max (to allow bouncing) or average.
        // We'll use max for energy conservation.
        return std::max(restitutionA, restitutionB);
    }

} // namespace USE