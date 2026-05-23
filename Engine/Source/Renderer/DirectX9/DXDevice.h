// ============================================================
// Ultimate Source Engine - DirectX 9 Render Device
// ============================================================
//
// Implements the IRenderDevice interface using DirectX 9.0c.
// Provides rendering context management and low-level drawing.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/IRenderDevice.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace USE {

    class DXDevice : public IRenderDevice {
    public:
        DXDevice();
        virtual ~DXDevice();

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

        // Resource creation
        bool CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle) override;
        bool CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle) override;
        bool CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle) override;

        void DestroyBuffer(uint32_t handle) override;
        void DestroyTexture(uint32_t handle) override;
        void DestroyShader(uint32_t handle) override;

        // Additional DirectX‑specific accessor
        IDirect3DDevice9* GetDevice() const { return m_device; }

    private:
        HWND                    m_hwnd;
        IDirect3D9*             m_d3d;
        IDirect3DDevice9*       m_device;
        D3DPRESENT_PARAMETERS   m_presentParams;
        bool                    m_initialized;
    };

} // namespace USE