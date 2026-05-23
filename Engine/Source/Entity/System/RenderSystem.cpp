// ============================================================
// Ultimate Source Engine - Render System Implementation
// ============================================================

#include "stdafx.h"
#include "RenderSystem.h"
#include "Entity/Entity.h"
#include "Entity/Components/RenderComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Renderer/Frustum.h"
#include "Core/Engine.h"
#include "Core/Logger.h"

namespace USE {

    RenderSystem::RenderSystem()
        : m_entityManager(nullptr)
        , m_componentManager(nullptr)
        , m_camera(nullptr)
    {
    }

    RenderSystem::~RenderSystem()
    {
        Shutdown();
    }

    void RenderSystem::Initialize(EntityManager* entityManager, ComponentManager* componentManager)
    {
        m_entityManager = entityManager;
        m_componentManager = componentManager;
    }

    void RenderSystem::Update(float deltaTime)
    {
        if (!m_entityManager || !m_componentManager || !m_camera) {
            return;
        }

        // Clear the render queue for the new frame
        m_renderQueue.Clear();

        // Compute frustum from camera for culling
        Frustum frustum;
        frustum.ExtractFromMatrix(m_camera->GetViewProjectionMatrix());

        // Get all entities (in a real ECS you'd have a view over entities with specific components)
        // For simplicity, we'll iterate over all entities and check if they have both components.
        // A more efficient approach would use the component manager's pools.
        std::vector<Entity*> entities = m_entityManager->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            RenderComponent* rc = entity->GetComponent<RenderComponent>();
            if (!rc || !rc->IsVisible()) continue;

            TransformComponent* tc = entity->GetComponent<TransformComponent>();
            if (!tc) continue;

            // Skip if mesh/material missing
            if (!rc->GetMesh() || !rc->GetMaterial()) continue;

            // Perform frustum culling if the render component has a bounding box
            // (we haven't added bounds to RenderComponent; we could later).
            // For now, we skip culling.

            // Add to render queue
            m_renderQueue.AddCommand(
                rc->GetMesh(),
                rc->GetMaterial(),
                tc->worldTransform.ToMatrix()
            );
        }

        // Sort the render queue (e.g., by material for batching)
        m_renderQueue.Sort(SortMode::ByMaterial, m_camera->GetPosition());
    }

    void RenderSystem::Shutdown()
    {
        m_renderQueue.Clear();
    }

} // namespace USE