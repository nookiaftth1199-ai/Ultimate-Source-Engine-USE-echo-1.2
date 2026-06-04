#pragma once

#include "Entity.h"
#include <vector>
#include <string>
#include <cstdint>

namespace USE
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		Entity* CreateEntity(const std::string& name = "Untitled");
		void DestroyEntity(Entity* entity);
		void DestroyAll();

		Entity* GetEntity(uint32_t id) const;
		const std::vector<Entity*>& GetAllEntities() const { return m_entities; }

	private:
		std::vector<Entity*> m_entities;
		uint32_t m_nextID = 0;
	};
}