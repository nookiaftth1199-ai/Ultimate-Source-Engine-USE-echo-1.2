// AnimationSystem.h
#pragma once
#include "Entity/Systems/SystemBase.h"
#include "Entity/EntityManager.h"
#include <vector>

namespace USE
{
	class Animator;

	class AnimationSystem : public SystemBase
	{
	public:
		explicit AnimationSystem(EntityManager* entityManager);
		void Update(float deltaTime) override;

	private:
		EntityManager* m_entityManager = nullptr;
	};
}