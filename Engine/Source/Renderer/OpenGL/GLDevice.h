// ============================================================
// Ultimate Source Engine - OpenGL Render Device
// ============================================================
//
// Implements the IRenderDevice interface using OpenGL 2.0+.
// Provides rendering context management and low-level drawing.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/IRenderDevice.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include <SDL.h>

namespace USE {

    class GLDevice : public IRenderDevice {
    public:
        GLDevice();
        virtual ~GLDevice();

        // IRenderDevice implementation
        bool Initialize(Window* window, bool vsync) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;

        void Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil) override;
        void SetViewport(int x, int y, int width, int height) override;
        void SetScissorRect(int x, int y, int width, int height) override;
        void EnableScissor(bool enable) override;

        void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation) override;

        // Resource creation (stubs for now)
        bool CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle) override;
        bool CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle) override;

        void DestroyBuffer(uint32_t handle) override;
        void DestroyTexture(uint32_t handle) override;
        void DestroyShader(uint32_t handle) override;

        // Additional OpenGL-specific methods
        const char* GetVendor() const;
        const char* GetRenderer() const;
        const char* GetVersion() const;

    private:
        SDL_GLContext m_context;
        SDL_Window*   m_window;
        bool          m_initialized;

        // OpenGL function pointers for extensions (if needed)
        // For OpenGL 2.0, most functions are core.

        // Helper methods
        void CheckGLError(const char* context);
    };

} // namespace USE