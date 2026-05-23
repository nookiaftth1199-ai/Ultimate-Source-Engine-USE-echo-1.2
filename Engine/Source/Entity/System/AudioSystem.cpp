// ============================================================
// Ultimate Source Engine - Audio System Implementation
// ============================================================

#include "stdafx.h"
#include "AudioSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/AudioComponent.h"

namespace USE {

    AudioSystem::AudioSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    AudioSystem::~AudioSystem()
    {
        Shutdown();
    }

    void AudioSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void AudioSystem::Update(float deltaTime)
    {
        if (!m_entityManager) return;

        // Iterate all entities and update those with AudioComponent
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            AudioComponent* ac = entity->GetComponent<AudioComponent>();
            if (ac) {
                ac->Update(deltaTime);
            }
        }
    }

    void AudioSystem::Shutdown()
    {
        // Nothing to clean up
    }

} // namespace USE