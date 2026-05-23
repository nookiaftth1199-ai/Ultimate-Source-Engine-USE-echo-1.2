// ============================================================
// Ultimate Source Engine - Scene
//============================================================
//
// Represents a game scene (level) containing a world of entities,
// a camera, lighting environment, skybox, and other global properties.
// ============================================================

#pragma once

#include "stdafx.h"
#include "World.h"
#include "Renderer/Camera.h"
#include "Renderer/Skybox.h"
#include "Renderer/Light.h"
#include <vector>

namespace USE {

    class Scene {
    public:
        Scene();
        ~Scene();

        // Initialize the scene (creates world and default camera)
        bool Initialize();
        void Shutdown();

        // Update all entities and scene elements
        void Update(float deltaTime);

        // Render the scene using the provided renderer
        void Render(class RenderSystem* renderer);

        // Access to the world (entity container)
        World* GetWorld() const { return m_world.get(); }

        // Camera management
        void SetMainCamera(Camera* camera);
        Camera* GetMainCamera() const { return m_mainCamera; }
        Camera* CreateCamera();

        // Skybox
        void SetSkybox(Skybox* skybox);
        Skybox* GetSkybox() const { return m_skybox; }

        // Lighting
        void AddLight(Light* light);
        void RemoveLight(Light* light);
        const std::vector<Light*>& GetLights() const { return m_lights; }

        // Ambient light
        void SetAmbientLight(const Color& color) { m_ambientLight = color; }
        const Color& GetAmbientLight() const { return m_ambientLight; }

    private:
        std::unique_ptr<World> m_world;
        Camera*                m_mainCamera;          // not owned (may be in world or separate)
        Skybox*                 m_skybox;               // owned by scene
        std::vector<Light*>     m_lights;               // owned by scene
        Color                   m_ambientLight;

        // Helper to create default camera if none set
        void EnsureMainCamera();
    };

} // namespace USE