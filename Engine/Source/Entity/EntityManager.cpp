#include "stdafx.h"
#include "EntityManager.h"
#include "Core/Logger.h"
#include <algorithm>

namespace USE
{
	EntityManager::EntityManager() = default;
	EntityManager::~EntityManager() { DestroyAll(); }

	Entity* EntityManager::CreateEntity(const std::string& name)
	{
		Entity* entity = new Entity(name);
		m_entities.push_back(entity);
		return entity;
	}

	void EntityManager::DestroyEntity(Entity* entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		if (it != m_entities.end())
		{
			delete *it;
			m_entities.erase(it);
		}
	}

	void EntityManager::DestroyAll()
	{
		for (Entity* e : m_entities)
			delete e;
		m_entities.clear();
	}

	Entity* EntityManager::GetEntity(uint32_t id) const
	{
		// For simplicity, index-based lookup (can be replaced with a map later)
		if (id < m_entities.size())
			return m_entities[id];
		return nullptr;
	}
}