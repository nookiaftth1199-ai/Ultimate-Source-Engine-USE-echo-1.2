// ============================================================
// Ultimate Source Engine - Physics System
// ============================================================
//
// System that synchronizes physics bodies with their entity transforms.
// Handles kinematic updates (entity → physics) and dynamic updates (physics → entity).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"

namespace USE {

    // Forward declarations
    class PhysicsWorld;

    class PhysicsSystem : public ISystem {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem();

        // Set the physics world used by this system.
        void SetPhysicsWorld(PhysicsWorld* world) { m_physicsWorld = world; }

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
        PhysicsWorld*     m_physicsWorld;
    };

} // namespace USE