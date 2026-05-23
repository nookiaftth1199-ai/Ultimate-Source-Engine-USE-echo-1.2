// ============================================================
// Ultimate Source Engine - Animation System Implementation
// ============================================================

#include "stdafx.h"
#include "AnimationSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/AnimationComponent.h"
#include "Entity/Components/SkeletalMeshComponent.h" // if exists
#include "Core/Logger.h"

namespace USE {

    AnimationSystem::AnimationSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
    {
    }

    AnimationSystem::~AnimationSystem()
    {
        Shutdown();
    }

    void AnimationSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void AnimationSystem::Update(float deltaTime)
    {
        if (!m_entityManager) return;

        // Iterate all entities and update those with AnimationComponent
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            AnimationComponent* animComp = entity->GetComponent<AnimationComponent>();
            if (animComp && animComp->IsEnabled()) {
                animComp->Update(deltaTime);
            }
        }
    }

    void AnimationSystem::Shutdown()
    {
        // Nothing to clean up
    }

} // namespace USE