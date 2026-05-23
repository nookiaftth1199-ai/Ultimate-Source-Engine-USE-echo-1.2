// ============================================================
// Ultimate Source Engine - AI System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "AISystem.h"
#include "AI/AIController.h"
#include "Entity/Entity.h"

namespace USE {

    AISystem::AISystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    AISystem::~AISystem()
    {
        Shutdown();
    }

    void AISystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void AISystem::Update(float deltaTime)
    {
        // Option 1: Use ECS – get all entities with AIControllerComponent (not shown here)
        // For now, we use the direct list of controllers.
        for (auto* ctrl : m_controllers) {
            ctrl->Update(deltaTime);
        }
    }

    void AISystem::Shutdown()
    {
        m_controllers.clear();
    }

    void AISystem::RegisterController(AIController* controller)
    {
        if (controller)
            m_controllers.push_back(controller);
    }

    void AISystem::UnregisterController(AIController* controller)
    {
        auto it = std::find(m_controllers.begin(), m_controllers.end(), controller);
        if (it != m_controllers.end())
            m_controllers.erase(it);
    }

} // namespace USE