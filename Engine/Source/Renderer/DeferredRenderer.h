// ============================================================
// Ultimate Source Engine - Deferred Renderer
// ============================================================
//
// Implements deferred shading: geometry pass (G‑buffer) followed by
// a lighting pass. Supports directional, point, and spot lights.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

namespace USE {

    class RenderSystem;
    class Camera;
    class Light;

    class DeferredRenderer {
    public:
        DeferredRenderer(RenderSystem* renderer);
        ~DeferredRenderer();

        // Initialize G‑buffer targets and shaders. Returns true on success.
        bool Initialize(int width, int height);
        void Shutdown();

        // Resize G‑buffer (called when window resizes)
        void Resize(int width, int height);

        // Perform geometry pass: render all opaque geometry, fill G‑buffer.
        void GeometryPass();

        // Perform lighting pass: compute lighting using G‑buffer and output to destination.
        void LightingPass(RenderTarget* destination);

        // Render transparent objects (forward) after lighting.
        void RenderTransparent();

        // Enable/disable deferred rendering (fallback to forward if disabled)
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        // Access G‑buffer textures (for debugging or post‑processing)
        RenderTarget* GetGBufferTarget(int index) const { return m_gBuffer[index]; }

    private:
        RenderSystem*   m_renderer;
        bool            m_enabled;
        int             m_width, m_height;

        // G‑buffer targets:
        // 0: Albedo (RGB) + roughness (A)
        // 1: Normal (XYZ) + metalness (A)
        // 2: World position (RGB) or depth + stencil
        // 3: Depth (if not using target 2)
        RenderTarget*   m_gBuffer[4];
        RenderTarget*   m_depthTarget;   // depth/stencil (shared with G‑buffer)

        // Fullscreen quad for lighting pass
        Mesh*           m_fullscreenQuad;

        // Shaders
        Shader*         m_geometryShader;   // writes to G‑buffer
        Shader*         m_lightingShader;   // reads G‑buffer, outputs final color

        // Helper: compile shaders, create quad mesh
        bool CreateShaders();
        void CreateFullscreenQuad();

        // Apply lighting for a single light (used in lighting pass)
        void ApplyLight(Light* light, const Matrix4& viewProj);
    };

} // namespace USE