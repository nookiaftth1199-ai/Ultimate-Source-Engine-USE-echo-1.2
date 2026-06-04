#pragma once

#include "../SystemManager.h"
#include "../EntityManager.h"

namespace USE
{
	class ScriptSystem : public SystemBase
	{
	public:
		explicit ScriptSystem(EntityManager* entityManager);
		void Update(float deltaTime) override;

	private:
		EntityManager* m_entityManager = nullptr;
	};
}