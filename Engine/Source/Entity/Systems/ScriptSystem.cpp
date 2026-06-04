#include "stdafx.h"
#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"

namespace USE
{
	ScriptSystem::ScriptSystem(EntityManager* entityManager)
		: m_entityManager(entityManager)
	{
	}

	void ScriptSystem::Update(float deltaTime)
	{
		if (!m_entityManager)
			return;

		for (Entity* entity : m_entityManager->GetAllEntities())
		{
			if (!entity || !entity->IsActive())
				continue;

			auto* script = entity->GetComponent<ScriptComponent>();
			if (script)
			{
				script->Update(deltaTime);
			}
		}
	}
}