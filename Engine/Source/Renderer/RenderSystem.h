// ============================================================
// Ultimate Source Engine - Render System
// ============================================================
//
// Manages rendering backends, render passes, and high‑level drawing.
// Supports multiple backends (OpenGL, DirectX9, Vulkan) and advanced features
// like deferred shading, clustered forward, planar reflections, and post‑processing.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Renderer/IRenderDevice.h"
#include "Renderer/RenderQueue.h"
#include "Renderer/RenderStats.h"
#include "Renderer/RenderTarget.h"

namespace USE {

    // Forward declarations
    class Window;
    class Camera;
    class Light;
    class Scene;
    class DeferredRenderer;
    class ClusteredForwardRenderer;
    class PostProcessManager;

    // Render backend enumeration
    enum class RenderBackend {
        OpenGL,
        DirectX9,
        Vulkan,
        AutoDetect
    };

    class RenderSystem {
    public:
        RenderSystem();
        ~RenderSystem();

        // Initialize with a backend and window
        bool Initialize(RenderBackend preferredBackend, Window* window, bool vsync = true);
        void Shutdown();

        // Backend management
        RenderBackend GetBackend() const { return m_backend; }
        const char* GetBackendName() const;
        IRenderDevice* GetDevice() const { return m_device; }

        // Frame control
        void BeginFrame();
        void EndFrame();
        void Present();

        // Clear buffers
        void Clear(uint32_t flags, const Color& color, float depth = 1.0f, uint32_t stencil = 0);

        // Viewport and scissor
        void SetViewport(int x, int y, int width, int height);
        void SetScissorRect(int x, int y, int width, int height);
        void EnableScissor(bool enable);

        // Rendering entry points
        void RenderScene(Scene* scene, Camera* camera);
        void RenderQueue(RenderQueue* queue, Camera* camera, bool transparent = false);

        // Post‑processing
        PostProcessManager* GetPostProcessManager() const { return m_postProcessManager; }

        // Advanced renderers (1.2 features)
        DeferredRenderer* GetDeferredRenderer() const { return m_deferredRenderer; }
        ClusteredForwardRenderer* GetClusteredForwardRenderer() const { return m_clusteredForwardRenderer; }

        // Enable/disable advanced features
        void SetDeferredEnabled(bool enabled);
        void SetClusteredForwardEnabled(bool enabled);
        void SetPlanarReflectionsEnabled(bool enabled);

        // Statistics
        const RenderStats& GetStats() const { return m_stats; }
        void ResetStats();

        // Factory for device creation
        static IRenderDevice* CreateDevice(RenderBackend backend, Window* window);

        // VSync control
        void SetVSync(bool enabled);
        bool IsVSyncEnabled() const;

    private:
        IRenderDevice*          m_device;
        RenderBackend           m_backend;
        RenderStats             m_stats;
        bool                    m_initialized;

        // Advanced renderers (1.2)
        DeferredRenderer*       m_deferredRenderer;
        ClusteredForwardRenderer* m_clusteredForwardRenderer;
        PostProcessManager*     m_postProcessManager;

        bool                    m_useDeferred;
        bool                    m_useClusteredForward;
        bool                    m_usePlanarReflections;

        // Internal helpers
        void CreateAdvancedRenderers(int width, int height);
        void DestroyAdvancedRenderers();
    };

} // namespace USE