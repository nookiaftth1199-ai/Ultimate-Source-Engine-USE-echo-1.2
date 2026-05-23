// ============================================================
// Ultimate Source Engine - Entity Manager
// ============================================================
//
// Manages all entities in the world: creation, destruction,
// updates, and queries.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity.h"
#include <unordered_map>
#include <memory>
#include <vector>

namespace USE {

    class EntityManager {
    public:
        EntityManager();
        ~EntityManager();

        // Create a new entity with optional name
        Entity* CreateEntity(const std::string& name = "Entity");

        // Destroy an entity by pointer
        void DestroyEntity(Entity* entity);

        // Destroy an entity by ID
        void DestroyEntity(uint64_t entityId);

        // Get entity by ID
        Entity* GetEntity(uint64_t entityId) const;

        // Get all entities
        std::vector<Entity*> GetAllEntities() const;

        // Get entities with a specific component type
        template<typename T>
        std::vector<Entity*> GetEntitiesWithComponent();

        // Update all active entities
        void Update(float deltaTime);

        // Clear all entities
        void Clear();

        // Get number of entities
        size_t GetEntityCount() const { return m_entities.size(); }

    private:
        std::unordered_map<uint64_t, std::unique_ptr<Entity>> m_entities;
    };

    // Template method implementation (in header)
    template<typename T>
    std::vector<Entity*> EntityManager::GetEntitiesWithComponent()
    {
        std::vector<Entity*> result;
        for (auto& pair : m_entities) {
            Entity* entity = pair.second.get();
            if (entity->IsActive() && entity->GetComponent<T>() != nullptr) {
                result.push_back(entity);
            }
        }
        return result;
    }

} // namespace USE