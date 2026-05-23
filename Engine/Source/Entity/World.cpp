// ============================================================
// Ultimate Source Engine - World Implementation
// ============================================================

#include "stdafx.h"
#include "World.h"

namespace USE {

    World::World()
        : m_entityManager(std::make_unique<EntityManager>())
        , m_systemManager(std::make_unique<SystemManager>())
        , m_initialized(false)
    {
    }

    World::~World()
    {
        Shutdown();
    }

    bool World::Initialize()
    {
        if (m_initialized) return true;

        // Initialize system manager (it needs entity manager to operate)
        m_systemManager->Initialize(m_entityManager.get(), nullptr); // second param may be unused in base

        m_initialized = true;
        return true;
    }

    void World::Shutdown()
    {
        if (!m_initialized) return;

        // Shutdown systems (clear them)
        m_systemManager->Clear();

        // Clear all entities
        m_entityManager->Clear();

        m_initialized = false;
    }

    void World::Update(float deltaTime)
    {
        if (!m_initialized) return;

        // Update all systems
        m_systemManager->Update(deltaTime);
    }

    Entity* World::CreateEntity(const std::string& name)
    {
        return m_entityManager->CreateEntity(name);
    }

    void World::DestroyEntity(Entity* entity)
    {
        m_entityManager->DestroyEntity(entity);
    }

    void World::DestroyEntity(uint64_t entityId)
    {
        m_entityManager->DestroyEntity(entityId);
    }

    Entity* World::GetEntity(uint64_t entityId) const
    {
        return m_entityManager->GetEntity(entityId);
    }

    std::vector<Entity*> World::GetAllEntities() const
    {
        return m_entityManager->GetAllEntities();
    }

} // namespace USE