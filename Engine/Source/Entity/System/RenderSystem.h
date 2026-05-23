// ============================================================
// Ultimate Source Engine - Render System
// ============================================================
//
// System that processes all entities with RenderComponent and
// TransformComponent, performing frustum culling and adding them
// to a render queue for later execution.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/SystemManager.h"
#include "Renderer/RenderQueue.h"
#include "Renderer/Camera.h"

namespace USE {

    class RenderSystem : public ISystem {
    public:
        RenderSystem();
        virtual ~RenderSystem();

        // ISystem interface
        void Initialize(EntityManager* entityManager, ComponentManager* componentManager) override;
        void Update(float deltaTime) override;
        void Shutdown() override;

        // Set the camera used for culling and view/proj matrices
        void SetCamera(Camera* camera) { m_camera = camera; }

        // Get the render queue (for external rendering)
        RenderQueue* GetRenderQueue() { return &m_renderQueue; }

    private:
        EntityManager*    m_entityManager;
        ComponentManager* m_componentManager;
        RenderQueue       m_renderQueue;
        Camera*           m_camera;
    };

} // namespace USE