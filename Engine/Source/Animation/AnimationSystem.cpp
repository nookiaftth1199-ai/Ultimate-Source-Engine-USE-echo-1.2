// AnimationSystem.cpp
#include "stdafx.h"
#include "AnimationSystem.h"
#include "Entity/Components/AnimatorComponent.h"   // we'll define this component
#include "Entity/Components/TransformComponent.h"
#include "Entity/Entity.h"

namespace USE
{
	AnimationSystem::AnimationSystem(EntityManager* entityManager)
		: m_entityManager(entityManager) {}

	void AnimationSystem::Update(float deltaTime)
	{
		if (!m_entityManager) return;
		for (Entity* entity : m_entityManager->GetAllEntities())
		{
			if (!entity || !entity->IsActive()) continue;
			auto* animator = entity->GetComponent<AnimatorComponent>();
			if (animator)
			{
				animator->GetAnimator().Update(deltaTime);
				// Optionally apply the resulting pose to the skeleton or mesh.
			}
		}
	}
}