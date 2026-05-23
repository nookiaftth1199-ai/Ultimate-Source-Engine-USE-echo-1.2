// ============================================================
// Ultimate Source Engine - Entity Manager Implementation
// ============================================================

#include "stdafx.h"
#include "EntityManager.h"

namespace USE {

    EntityManager::EntityManager()
    {
    }

    EntityManager::~EntityManager()
    {
        Clear();
    }

    Entity* EntityManager::CreateEntity(const std::string& name)
    {
        auto entity = std::make_unique<Entity>(name);
        Entity* raw = entity.get();
        m_entities[raw->GetID()] = std::move(entity);
        return raw;
    }

    void EntityManager::DestroyEntity(Entity* entity)
    {
        if (entity) {
            DestroyEntity(entity->GetID());
        }
    }

    void EntityManager::DestroyEntity(uint64_t entityId)
    {
        auto it = m_entities.find(entityId);
        if (it != m_entities.end()) {
            // Optionally call OnDespawn on entity before destruction
            it->second->OnDespawn();
            m_entities.erase(it);
        }
    }

    Entity* EntityManager::GetEntity(uint64_t entityId) const
    {
        auto it = m_entities.find(entityId);
        return (it != m_entities.end()) ? it->second.get() : nullptr;
    }

    std::vector<Entity*> EntityManager::GetAllEntities() const
    {
        std::vector<Entity*> result;
        result.reserve(m_entities.size());
        for (auto& pair : m_entities) {
            result.push_back(pair.second.get());
        }
        return result;
    }

    void EntityManager::Update(float deltaTime)
    {
        // Iterate over a snapshot of entities in case one destroys itself during update
        std::vector<Entity*> entities = GetAllEntities();
        for (Entity* entity : entities) {
            if (entity && entity->IsActive()) {
                entity->Update(deltaTime);
            }
        }
    }

    void EntityManager::Clear()
    {
        m_entities.clear();
    }

} // namespace USE