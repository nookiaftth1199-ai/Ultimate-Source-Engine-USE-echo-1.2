// ============================================================
// Ultimate Source Engine - World
// ============================================================
//
// The World class is the top-level container for all game entities
// and systems. It owns the EntityManager and SystemManager and
// provides a unified interface for creating and updating the game world.
// ============================================================

#pragma once

#include "stdafx.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include <memory>

namespace USE {

    class World {
    public:
        World();
        ~World();

        // Initialize the world (creates managers)
        bool Initialize();
        void Shutdown();

        // Update all systems
        void Update(float deltaTime);

        // Entity creation/destruction (forwarded to EntityManager)
        Entity* CreateEntity(const std::string& name = "Entity");
        void DestroyEntity(Entity* entity);
        void DestroyEntity(uint64_t entityId);

        // Get entity by ID
        Entity* GetEntity(uint64_t entityId) const;

        // Get all entities
        std::vector<Entity*> GetAllEntities() const;

        // System management
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args);

        template<typename T>
        T* GetSystem() const;

        // Access to managers (if needed)
        EntityManager* GetEntityManager() const { return m_entityManager.get(); }
        SystemManager* GetSystemManager() const { return m_systemManager.get(); }

    private:
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<SystemManager> m_systemManager;
        bool m_initialized;
    };

    // -----------------------------------------------------------------
    // Template implementations
    // -----------------------------------------------------------------
    template<typename T, typename... Args>
    T* World::RegisterSystem(Args&&... args)
    {
        return m_systemManager->RegisterSystem<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    T* World::GetSystem() const
    {
        return m_systemManager->GetSystem<T>();
    }

} // namespace USE