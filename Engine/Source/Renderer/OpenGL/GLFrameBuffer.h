// ============================================================
// Ultimate Source Engine - OpenGL Framebuffer Object
// ============================================================
//
// Encapsulates an OpenGL framebuffer object (FBO) for off‑screen rendering.
// Supports color and depth/stencil attachments.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Renderer/RenderTypes.h"

namespace USE {

    class GLTexture;

    class GLFramebuffer {
    public:
        GLFramebuffer();
        ~GLFramebuffer();

        // Create a framebuffer with the given color and depth attachments.
        // The textures are created internally; use GetColorTexture() etc. to access them.
        bool Create(int width, int height, TextureFormat colorFormat = TextureFormat::RGBA8_UNORM,
                    bool createDepthStencil = true, TextureFormat depthFormat = TextureFormat::D24_UNORM_S8_UINT);

        // Attach existing textures (useful for custom setups).
        bool AttachColorTexture(GLTexture* texture, int attachmentIndex = 0);
        bool AttachDepthStencilTexture(GLTexture* texture);

        // Bind/unbind the framebuffer.
        void Bind();
        void Unbind();

        // Check if framebuffer is complete.
        bool IsComplete() const;

        // Get the internal OpenGL handle.
        unsigned int GetHandle() const { return m_fbo; }

        // Get attached textures (if created internally).
        GLTexture* GetColorTexture(int index = 0) const;
        GLTexture* GetDepthStencilTexture() const;

        // Resize the framebuffer (recreates attachments with new size).
        bool Resize(int width, int height);

        // Clear the attached buffers.
        void Clear(uint32_t flags, const Color& color = Color::Black, float depth = 1.0f, uint32_t stencil = 0);

        // Get dimensions.
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        unsigned int m_fbo;
        int m_width;
        int m_height;

        // Internal textures (if we created them)
        GLTexture* m_colorAttachments[8]; // support up to 8 MRTs
        GLTexture* m_depthStencilAttachment;

        int m_numColorAttachments;

        void Destroy();
        bool CheckStatus();
    };

} // namespace USE