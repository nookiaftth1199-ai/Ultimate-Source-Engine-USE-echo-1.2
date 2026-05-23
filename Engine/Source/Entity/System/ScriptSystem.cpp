// ============================================================
// Ultimate Source Engine - Script System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ScriptSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/ScriptComponent.h"

namespace USE {

    ScriptSystem::ScriptSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    ScriptSystem::~ScriptSystem()
    {
        Shutdown();
    }

    void ScriptSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void ScriptSystem::Update(float deltaTime)
    {
        if (!m_entityManager) return;

        // Get all entities (simplified – in a real ECS you'd use a view)
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            ScriptComponent* sc = entity->GetComponent<ScriptComponent>();
            if (sc && sc->HasScript()) {
                sc->Update(deltaTime);
            }
        }
    }

    void ScriptSystem::Shutdown()
    {
        // Nothing to clean up
    }

} // namespace USE