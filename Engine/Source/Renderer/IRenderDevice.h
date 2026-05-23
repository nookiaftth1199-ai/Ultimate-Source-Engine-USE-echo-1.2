// ============================================================
// Ultimate Source Engine - Render Device Interface
// ============================================================
//
// Abstract interface for rendering backends (OpenGL, DirectX 9, Vulkan).
// Defines the methods that each concrete device must implement.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Math/Viewport.h"
#include "Math/Rect.h"
#include "Renderer/RenderTypes.h"

namespace USE {

    // Forward declarations
    class Window;

    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;

        // -----------------------------------------------------------------
        // Device lifecycle
        // -----------------------------------------------------------------
        virtual bool Initialize(Window* window, bool vsync) = 0;
        virtual void Shutdown() = 0;

        // -----------------------------------------------------------------
        // Frame control
        // -----------------------------------------------------------------
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;

        // -----------------------------------------------------------------
        // Clearing
        // -----------------------------------------------------------------
        virtual void Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil) = 0;

        // -----------------------------------------------------------------
        // Viewport and scissor
        // -----------------------------------------------------------------
        virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual void SetScissorRect(int x, int y, int width, int height) = 0;
        virtual void EnableScissor(bool enable) = 0;

        // -----------------------------------------------------------------
        // Drawing
        // -----------------------------------------------------------------
        virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation,
                                 uint32_t baseVertexLocation) = 0;
        virtual void DrawArrays(uint32_t vertexCount, uint32_t startVertexLocation) = 0;

        // -----------------------------------------------------------------
        // Resource creation/destruction
        // -----------------------------------------------------------------
        // Buffers
        virtual bool CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle) = 0;
        virtual bool CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle) = 0;
        virtual void DestroyBuffer(uint32_t bufferHandle) = 0;

        // Textures
        virtual bool CreateTexture2D(int width, int height, TextureFormat format,
                                     const void* data, uint32_t& textureHandle) = 0;
        virtual void DestroyTexture(uint32_t textureHandle) = 0;

        // Shaders
        virtual bool CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle) = 0;
        virtual void DestroyShader(uint32_t shaderHandle) = 0;

        // Shader programs (for backends that separate shader objects from programs)
        virtual bool CreateShaderProgram(uint32_t vertexShader, uint32_t fragmentShader,
                                          uint32_t geometryShader, uint32_t& programHandle) = 0;
        virtual void DestroyShaderProgram(uint32_t programHandle) = 0;

        // Framebuffers (render targets)
        virtual bool CreateFramebuffer(uint32_t colorTextureHandle, uint32_t depthStencilTextureHandle,
                                       uint32_t& framebufferHandle) = 0;
        virtual void DestroyFramebuffer(uint32_t framebufferHandle) = 0;

        // Samplers (optional)
        virtual bool CreateSampler(const SamplerDesc& desc, uint32_t& samplerHandle) = 0;
        virtual void DestroySampler(uint32_t samplerHandle) = 0;

        // -----------------------------------------------------------------
        // Resource binding
        // -----------------------------------------------------------------
        virtual void SetVertexBuffer(uint32_t slot, uint32_t bufferHandle,
                                     uint32_t stride, uint32_t offset) = 0;
        virtual void SetIndexBuffer(uint32_t bufferHandle, uint32_t offset) = 0;
        virtual void SetTexture(uint32_t unit, uint32_t textureHandle) = 0;
        virtual void SetSampler(uint32_t unit, uint32_t samplerHandle) = 0;
        virtual void SetShaderProgram(uint32_t programHandle) = 0;

        // -----------------------------------------------------------------
        // Render states
        // -----------------------------------------------------------------
        virtual void SetDepthStencilState(const DepthStencilDesc& desc) = 0;
        virtual void SetRasterizerState(const RasterizerDesc& desc) = 0;
        virtual void SetBlendState(const BlendAttachmentDesc* attachments, uint32_t count) = 0;

        // -----------------------------------------------------------------
        // Device information
        // -----------------------------------------------------------------
        virtual const char* GetDeviceName() const = 0;
        virtual const char* GetAPIName() const = 0;
    };

} // namespace USE