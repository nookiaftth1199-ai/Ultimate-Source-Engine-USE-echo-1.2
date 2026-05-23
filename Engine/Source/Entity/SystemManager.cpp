// ============================================================
// Ultimate Source Engine - System Manager Implementation
// ============================================================

#include "stdafx.h"
#include "SystemManager.h"

namespace USE {

    SystemManager::SystemManager()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    SystemManager::~SystemManager()
    {
        Clear();
    }

    void SystemManager::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void SystemManager::Update(float deltaTime)
    {
        for (auto& system : m_systems) {
            if (system->IsEnabled()) {
                system->Update(deltaTime);
            }
        }
    }

    void SystemManager::SetAllEnabled(bool enabled)
    {
        for (auto& system : m_systems) {
            system->SetEnabled(enabled);
        }
    }

    void SystemManager::Clear()
    {
        // Call Shutdown on all systems before destruction
        for (auto& system : m_systems) {
            system->Shutdown();
        }
        m_systems.clear();
    }

} // namespace USE