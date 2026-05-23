// ============================================================
// Ultimate Source Engine - Component Manager Implementation
// ============================================================

#include "stdafx.h"
#include "ComponentManager.h"

namespace USE {

    ComponentManager::ComponentManager()
        : m_nextEntityId(0)
    {
        // Pre-allocate some space for entity tracking
        m_entityAlive.reserve(1024);
    }

    uint32_t ComponentManager::CreateEntity()
    {
        uint32_t id;
        if (!m_freeEntities.empty()) {
            id = m_freeEntities.back();
            m_freeEntities.pop_back();
            m_entityAlive[id] = true;
        } else {
            id = m_nextEntityId++;
            m_entityAlive.push_back(true);
        }
        return id;
    }

    void ComponentManager::DestroyEntity(uint32_t entity)
    {
        if (!IsAlive(entity)) return;

        // Notify all pools that this entity is gone
        for (auto& pair : m_pools) {
            pair.second->EntityDestroyed(entity);
        }

        m_entityAlive[entity] = false;
        m_freeEntities.push_back(entity);
    }

    bool ComponentManager::IsAlive(uint32_t entity) const
    {
        return entity < m_entityAlive.size() && m_entityAlive[entity];
    }

} // namespace USE