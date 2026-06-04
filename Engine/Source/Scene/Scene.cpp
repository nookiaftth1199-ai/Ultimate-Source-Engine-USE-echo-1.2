// ============================================================// ============================================================
// Ultimate Source Engine - Scene Implementation
// ============================================================

#include "stdafx.h"
#include "Scene.h"

namespace USE
{
	Scene::Scene() = default;
	Scene::~Scene() = default;

	Entity* Scene::CreateEntity(const std::string& name)
	{
		Entity* entity = m_world.CreateEntity(name);
		m_entities.push_back(entity);
		return entity;
	}

	void Scene::DestroyEntity(Entity* entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		if (it != m_entities.end())
		{
			m_world.DestroyEntity(entity);
			m_entities.erase(it);
		}
	}
}