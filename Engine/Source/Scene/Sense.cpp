// ============================================================
// Ultimate Source Engine - Scene Implementation
// ============================================================

#include "stdafx.h"
#include "Scene.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/RenderQueue.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/RenderComponent.h"
#include "Core/Logger.h"

namespace USE {

    Scene::Scene()
        : m_world(std::make_unique<World>())
        , m_mainCamera(nullptr)
        , m_skybox(nullptr)
        , m_ambientLight(0.1f, 0.1f, 0.1f, 1.0f)
    {
    }

    Scene::~Scene()
    {
        Shutdown();
    }

    bool Scene::Initialize()
    {
        if (!m_world->Initialize()) {
            USE_LOG_ERROR("Scene: Failed to initialize world");
            return false;
        }

        // Create a default camera if none exists
        EnsureMainCamera();

        return true;
    }

    void Scene::Shutdown()
    {
        // Delete owned objects
        for (auto* light : m_lights) {
            delete light;
        }
        m_lights.clear();

        if (m_skybox) {
            delete m_skybox;
            m_skybox = nullptr;
        }

        m_world->Shutdown();
    }

    void Scene::Update(float deltaTime)
    {
        m_world->Update(deltaTime);
    }

    void Scene::Render(RenderSystem* renderer)
    {
        if (!renderer || !m_mainCamera) return;

        // Clear screen (will be done by renderer's begin frame)
        // Set up lights for shaders? Not yet.

        // Render skybox first (if any)
        if (m_skybox) {
            m_skybox->Render(m_mainCamera);
        }

        // Get render queue from world? We need to collect all renderable entities.
        // For now, we'll rely on a hypothetical RenderSystem in the world.
        // But World doesn't have a built-in render system. We could add one.
        // Alternatively, we can iterate entities and manually add to queue.
        // Let's use a RenderQueue local to this method.

        RenderQueue queue;

        // Collect all entities with RenderComponent and TransformComponent
        auto entities = m_world->GetAllEntities();
        for (Entity* entity : entities) {
            if (!entity->IsActive()) continue;

            RenderComponent* rc = entity->GetComponent<RenderComponent>();
            if (!rc || !rc->IsVisible()) continue;

            TransformComponent* tc = entity->GetComponent<TransformComponent>();
            if (!tc) continue;

            if (!rc->GetMesh() || !rc->GetMaterial()) continue;

            queue.AddCommand(rc->GetMesh(), rc->GetMaterial(),
                             tc->worldTransform.ToMatrix());
        }

        // Sort and execute
        queue.Sort(SortMode::ByMaterial, m_mainCamera->GetPosition());
        queue.Execute(renderer->GetDevice());
    }

    void Scene::SetMainCamera(Camera* camera)
    {
        m_mainCamera = camera;
    }

    Camera* Scene::CreateCamera()
    {
        // In a more advanced version, you might create an entity with a CameraComponent.
        // For simplicity, we just create a standalone Camera object.
        Camera* cam = new Camera();
        // Add to some list? Not yet.
        if (!m_mainCamera) {
            m_mainCamera = cam;
        }
        return cam;
    }

    void Scene::SetSkybox(Skybox* skybox)
    {
        if (m_skybox) delete m_skybox;
        m_skybox = skybox;
    }

    void Scene::AddLight(Light* light)
    {
        m_lights.push_back(light);
    }

    void Scene::RemoveLight(Light* light)
    {
        auto it = std::find(m_lights.begin(), m_lights.end(), light);
        if (it != m_lights.end()) {
            m_lights.erase(it);
            delete light;
        }
    }

    void Scene::EnsureMainCamera()
    {
        if (!m_mainCamera) {
            m_mainCamera = CreateCamera();
        }
    }

} // namespace USE