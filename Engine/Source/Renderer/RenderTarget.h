// ============================================================
// Ultimate Source Engine - Render Target
// ============================================================
//
// Encapsulates a framebuffer object with color and optional depth/stencil
// attachments. Provides methods to create, bind, and access the attached textures.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/Texture.h"

namespace USE {

    class IRenderDevice;

    class RenderTarget {
    public:
        RenderTarget();
        ~RenderTarget();

        // Create a render target with given dimensions and formats.
        // If depthStencil is false, no depth/stencil attachment is created.
        bool Create(IRenderDevice* device,
                    int width, int height,
                    TextureFormat colorFormat = TextureFormat::RGBA8_UNORM,
                    bool depthStencil = true,
                    TextureFormat depthFormat = TextureFormat::D24_UNORM_S8_UINT);

        // Destroy the render target (releases GPU resources)
        void Destroy();

        // Bind the render target for rendering (all subsequent draw calls go to it)
        void Bind();

        // Unbind (restore backbuffer)
        void Unbind();

        // Clear the attached buffers (color, depth, stencil) using provided values
        void Clear(uint32_t flags,
                   const Color& color = Color::Black,
                   float depth = 1.0f,
                   uint32_t stencil = 0);

        // Get attached textures (owned by the render target, do not delete)
        Texture* GetColorTexture() const { return m_colorTexture; }
        Texture* GetDepthStencilTexture() const { return m_depthStencilTexture; }

        // Get dimensions
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        // Check if valid
        bool IsValid() const { return m_framebufferHandle != 0; }

    private:
        IRenderDevice* m_device;
        uint32_t       m_framebufferHandle; // opaque handle from device
        Texture*       m_colorTexture;
        Texture*       m_depthStencilTexture;
        int            m_width;
        int            m_height;
        bool           m_ownsTextures; // whether we created the textures (always true for now)
    };

} // namespace USE