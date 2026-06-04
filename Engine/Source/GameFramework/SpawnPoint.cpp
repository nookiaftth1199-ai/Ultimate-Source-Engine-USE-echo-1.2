// ============================================================
// Ultimate Source Engine - Spawn Point Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "SpawnPoint.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"
#include "Core/Engine.h"
#include "Physics/PhysicsWorld.h"

namespace USE {

    SpawnPoint::SpawnPoint()
        : m_teamId(0)
        , m_priority(0)
        , m_occupied(false)
    {
    }

    SpawnPoint::~SpawnPoint()
    {
    }

    bool SpawnPoint::IsValid() const
    {
        if (m_occupied) return false;

        // Check if there is an entity occupying the space (optional)
        Entity* owner = GetOwner();
        if (!owner) return false;

        TransformComponent* tc = owner->GetComponent<TransformComponent>();
        if (!tc) return false;

        // Simple check: raycast or sphere test for entities at spawn position
        // For now, just check occupancy flag.
        return true;
    }

} // namespace USE