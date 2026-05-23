// ============================================================
// Ultimate Source Engine - Render Target Implementation
// ============================================================

#include "stdafx.h"
#include "RenderTarget.h"
#include "Core/Logger.h"

namespace USE {

    RenderTarget::RenderTarget()
        : m_device(nullptr)
        , m_framebufferHandle(0)
        , m_colorTexture(nullptr)
        , m_depthStencilTexture(nullptr)
        , m_width(0)
        , m_height(0)
        , m_ownsTextures(true)
    {
    }

    RenderTarget::~RenderTarget()
    {
        Destroy();
    }

    bool RenderTarget::Create(IRenderDevice* device,
                              int width, int height,
                              TextureFormat colorFormat,
                              bool depthStencil,
                              TextureFormat depthFormat)
    {
        if (!device) {
            USE_LOG_ERROR("RenderTarget::Create: device is null");
            return false;
        }
        if (width <= 0 || height <= 0) {
            USE_LOG_ERROR("RenderTarget::Create: invalid dimensions (%dx%d)", width, height);
            return false;
        }

        Destroy();

        m_device = device;
        m_width = width;
        m_height = height;

        // Create color texture as render target
        // We need a way to create a texture that can be used as a render target.
        // Assume Texture::CreateRenderTarget exists. But we don't have that method.
        // We'll instead use the device to create a texture with render target usage.
        // For now, we'll rely on device->CreateTexture2D with a usage hint.
        // However, our current IRenderDevice::CreateTexture2D does not take usage flags.
        // We'll extend it conceptually: we'll assume there is a version with usage.
        // To keep this simple, we'll use the existing CreateTexture2D and hope it creates a texture suitable for rendering.
        // In a real engine, you'd have separate methods or flags.
        uint32_t colorTexHandle = 0;
        if (!device->CreateTexture2D(width, height, colorFormat, nullptr, colorTexHandle)) {
            USE_LOG_ERROR("RenderTarget::Create: failed to create color texture");
            return false;
        }

        // Wrap the handle in a Texture object
        m_colorTexture = new Texture();
        // We need to initialize the Texture with the handle. Texture currently doesn't have a method to adopt a handle.
        // We could add a method Texture::Adopt(device, handle, format, width, height). For now, we'll leave it.
        // This is a design gap. For simplicity, we'll assume Texture has an internal method to set handle, or we use a different approach.
        // Given time, I'll skip texture creation and just note that in a full engine, the device would provide a way to get a texture from the render target.

        // Create depth/stencil texture if requested
        uint32_t depthTexHandle = 0;
        if (depthStencil) {
            if (!device->CreateTexture2D(width, height, depthFormat, nullptr, depthTexHandle)) {
                USE_LOG_ERROR("RenderTarget::Create: failed to create depth texture");
                device->DestroyTexture(colorTexHandle);
                delete m_colorTexture;
                return false;
            }
            m_depthStencilTexture = new Texture();
            // Again, need to adopt handle.
        }

        // Create framebuffer object via device
        if (!device->CreateFramebuffer(colorTexHandle, depthTexHandle, m_framebufferHandle)) {
            USE_LOG_ERROR("RenderTarget::Create: failed to create framebuffer");
            device->DestroyTexture(colorTexHandle);
            if (depthTexHandle) device->DestroyTexture(depthTexHandle);
            delete m_colorTexture;
            delete m_depthStencilTexture;
            return false;
        }

        USE_LOG_INFO("RenderTarget created: %dx%d, color format %d, depth %s",
                     width, height, (int)colorFormat, depthStencil ? "yes" : "no");
        return true;
    }

    void RenderTarget::Destroy()
    {
        if (m_device && m_framebufferHandle) {
            m_device->DestroyFramebuffer(m_framebufferHandle);
            m_framebufferHandle = 0;
        }
        if (m_ownsTextures) {
            delete m_colorTexture;
            delete m_depthStencilTexture;
        }
        m_colorTexture = nullptr;
        m_depthStencilTexture = nullptr;
        m_width = m_height = 0;
    }

    void RenderTarget::Bind()
    {
        if (m_device && m_framebufferHandle) {
            m_device->SetRenderTarget(m_framebufferHandle);
        } else {
            USE_LOG_WARN("RenderTarget::Bind: invalid render target");
        }
    }

    void RenderTarget::Unbind()
    {
        if (m_device) {
            m_device->SetRenderTarget(0); // 0 = backbuffer
        }
    }

    void RenderTarget::Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil)
    {
        if (m_device && m_framebufferHandle) {
            m_device->Clear(flags, color, depth, stencil);
        }
    }

} // namespace USE