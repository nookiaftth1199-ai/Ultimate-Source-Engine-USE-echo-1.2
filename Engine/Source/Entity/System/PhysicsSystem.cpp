// ============================================================
// Ultimate Source Engine - Physics System Implementation
// ============================================================

#include "stdafx.h"
#include "PhysicsSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Physics/PhysicsWorld.h"
#include "Core/Logger.h"

namespace USE {

    PhysicsSystem::PhysicsSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
        , m_physicsWorld(nullptr)
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
        Shutdown();
    }

    void PhysicsSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void PhysicsSystem::Update(float deltaTime)
    {
        if (!m_entityManager || !m_componentManager || !m_physicsWorld) return;

        // First, update kinematic bodies: copy entity transform to physics body.
        // In a more efficient ECS, you'd iterate over a view of entities with both components.
        // For simplicity, we iterate all entities.
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            PhysicsComponent* pc = entity->GetComponent<PhysicsComponent>();
            if (!pc || !pc->IsValid()) continue;

            // For kinematic bodies, we push entity transform to physics.
            // (Dynamic bodies are handled by the physics world and pulled later.)
            // But we can also let the PhysicsComponent handle its own updates.
            // Since PhysicsComponent already has Update/LateUpdate, we could rely on that.
            // However, to centralize, we'll just let the component do its own thing.
            // Actually, PhysicsComponent already implements Update and LateUpdate.
            // So we don't need to do anything here; the component handles itself.
            // The system could be responsible for calling component updates, but they are called automatically by the World.
            // So this system might be redundant if we just rely on components.
            // But we'll keep it as a placeholder for now.
        }
    }

    void PhysicsSystem::Shutdown()
    {
        // Nothing to clean up
    }

} // namespace USE