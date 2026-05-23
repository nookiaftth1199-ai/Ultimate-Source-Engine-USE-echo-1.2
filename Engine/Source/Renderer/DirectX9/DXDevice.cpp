// ============================================================
// Ultimate Source Engine - DirectX 9 Device Implementation
// ============================================================

#include "stdafx.h"
#include "DXDevice.h"
#include "Core/Window.h"
#include "Core/Logger.h"

namespace USE {

    DXDevice::DXDevice()
        : m_hwnd(nullptr)
        , m_d3d(nullptr)
        , m_device(nullptr)
        , m_initialized(false)
    {
        ZeroMemory(&m_presentParams, sizeof(m_presentParams));
    }

    DXDevice::~DXDevice()
    {
        Shutdown();
    }

    bool DXDevice::Initialize(Window* window, bool vsync)
    {
        if (m_initialized) return true;

        USE_LOG_INFO("Initializing DirectX 9 device...");

        // Retrieve native window handle (must be HWND on Windows)
        m_hwnd = (HWND)window->GetNativeHandle();
        if (!m_hwnd) {
            USE_LOG_ERROR("DXDevice: Invalid window handle");
            return false;
        }

        // Create the Direct3D object
        m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!m_d3d) {
            USE_LOG_ERROR("DXDevice: Failed to create Direct3D9 object");
            return false;
        }

        // Get current display mode (to pick a compatible back buffer format)
        D3DDISPLAYMODE d3ddm;
        if (FAILED(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) {
            USE_LOG_ERROR("DXDevice: Failed to get adapter display mode");
            return false;
        }

        // Set up presentation parameters
        ZeroMemory(&m_presentParams, sizeof(m_presentParams));
        m_presentParams.BackBufferWidth = window->GetWidth();
        m_presentParams.BackBufferHeight = window->GetHeight();
        m_presentParams.BackBufferFormat = d3ddm.Format;
        m_presentParams.BackBufferCount = 1;
        m_presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
        m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        m_presentParams.hDeviceWindow = m_hwnd;
        m_presentParams.Windowed = TRUE;               // Windowed mode (can be changed later)
        m_presentParams.EnableAutoDepthStencil = TRUE;
        m_presentParams.AutoDepthStencilFormat = D3DFMT_D24S8; // 24‑bit depth + 8‑bit stencil
        m_presentParams.Flags = 0;
        m_presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        m_presentParams.PresentationInterval = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

        // Create the device
        if (FAILED(m_d3d->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL,
                m_hwnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                &m_presentParams,
                &m_device))) {
            USE_LOG_ERROR("DXDevice: Failed to create device");
            return false;
        }

        m_initialized = true;
        USE_LOG_INFO("DirectX 9 device initialized successfully.");
        return true;
    }

    void DXDevice::Shutdown()
    {
        if (!m_initialized) return;

        USE_LOG_INFO("Shutting down DirectX 9 device...");

        if (m_device) {
            m_device->Release();
            m_device = nullptr;
        }
        if (m_d3d) {
            m_d3d->Release();
            m_d3d = nullptr;
        }

        m_initialized = false;
        USE_LOG_INFO("DirectX 9 device shut down.");
    }

    void DXDevice::BeginFrame()
    {
        // Nothing specific required for DX9
    }

    void DXDevice::EndFrame()
    {
        // Nothing specific required for DX9
    }

    void DXDevice::Present()
    {
        if (m_device) {
            m_device->Present(nullptr, nullptr, nullptr, nullptr);
        }
    }

    void DXDevice::Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil)
    {
        if (!m_device) return;

        DWORD clearFlags = 0;
        if (flags & CLEAR_COLOR)   clearFlags |= D3DCLEAR_TARGET;
        if (flags & CLEAR_DEPTH)   clearFlags |= D3DCLEAR_ZBUFFER;
        if (flags & CLEAR_STENCIL) clearFlags |= D3DCLEAR_STENCIL;

        D3DCOLOR d3dColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
        m_device->Clear(0, nullptr, clearFlags, d3dColor, depth, stencil);
    }

    void DXDevice::SetViewport(int x, int y, int width, int height)
    {
        D3DVIEWPORT9 vp;
        vp.X = x;
        vp.Y = y;
        vp.Width = width;
        vp.Height = height;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
        m_device->SetViewport(&vp);
    }

    void DXDevice::SetScissorRect(int x, int y, int width, int height)
    {
        RECT rect = { x, y, x + width, y + height };
        m_device->SetScissorRect(&rect);
    }

    void DXDevice::EnableScissor(bool enable)
    {
        m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, enable ? TRUE : FALSE);
    }

    void DXDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
    {
        // In DirectX 9, DrawIndexedPrimitive uses StartIndex and BaseVertexIndex.
        // The primitive type is assumed to be triangle list (can be made configurable).
        m_device->DrawIndexedPrimitive(
            D3DPT_TRIANGLELIST,
            baseVertexLocation,      // vertex offset added to each index
            0,                       // minimum vertex index (can be 0)
            0,                       // number of vertices (can be 0 if not needed)
            startIndexLocation,
            indexCount / 3            // primitive count
        );
    }

    bool DXDevice::CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        IDirect3DVertexBuffer9* vb = nullptr;
        if (FAILED(m_device->CreateVertexBuffer(
                (UINT)size,
                0,                    // usage
                0,                    // FVF (not used with shaders)
                D3DPOOL_MANAGED,
                &vb,
                nullptr))) {
            USE_LOG_ERROR("DXDevice: Failed to create vertex buffer");
            return false;
        }

        if (data) {
            void* ptr;
            if (SUCCEEDED(vb->Lock(0, 0, &ptr, 0))) {
                memcpy(ptr, data, size);
                vb->Unlock();
            }
        }

        bufferHandle = (uint32_t)(size_t)vb; // store pointer as handle (simple cast)
        return true;
    }

    bool DXDevice::CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        IDirect3DIndexBuffer9* ib = nullptr;
        if (FAILED(m_device->CreateIndexBuffer(
                (UINT)size,
                0,                    // usage
                D3DFMT_INDEX32,        // 32‑bit indices (or D3DFMT_INDEX16)
                D3DPOOL_MANAGED,
                &ib,
                nullptr))) {
            USE_LOG_ERROR("DXDevice: Failed to create index buffer");
            return false;
        }

        if (data) {
            void* ptr;
            if (SUCCEEDED(ib->Lock(0, 0, &ptr, 0))) {
                memcpy(ptr, data, size);
                ib->Unlock();
            }
        }

        bufferHandle = (uint32_t)(size_t)ib;
        return true;
    }

    bool DXDevice::CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle)
    {
        // For simplicity, we assume RGBA8_UNORM maps to D3DFMT_A8R8G8B8.
        // A full implementation would map TextureFormat to D3DFORMAT properly.
        D3DFORMAT d3dFormat = D3DFMT_A8R8G8B8;
        // (Add mapping code here if needed)

        IDirect3DTexture9* tex = nullptr;
        if (FAILED(m_device->CreateTexture(
                width, height,
                1,                     // mip levels
                0,                     // usage
                d3dFormat,
                D3DPOOL_MANAGED,
                &tex,
                nullptr))) {
            USE_LOG_ERROR("DXDevice: Failed to create texture");
            return false;
        }

        if (data) {
            D3DLOCKED_RECT lockedRect;
            if (SUCCEEDED(tex->LockRect(0, &lockedRect, nullptr, 0))) {
                // Copy row by row – assume packed RGBA8
                memcpy(lockedRect.pBits, data, width * height * 4);
                tex->UnlockRect(0);
            }
        }

        textureHandle = (uint32_t)(size_t)tex;
        return true;
    }

    bool DXDevice::CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle)
    {
        // DirectX 9 uses vertex shaders and pixel shaders (assembler or HLSL via D3DX).
        // This is a placeholder; a real implementation would compile and create
        // an IDirect3DVertexShader9 or IDirect3DPixelShader9.
        USE_LOG_WARN("DXDevice::CreateShader not implemented");
        shaderHandle = 0;
        return false;
    }

    void DXDevice::DestroyBuffer(uint32_t handle)
    {
        IUnknown* obj = (IUnknown*)(size_t)handle;
        if (obj) obj->Release();
    }

    void DXDevice::DestroyTexture(uint32_t handle)
    {
        IUnknown* obj = (IUnknown*)(size_t)handle;
        if (obj) obj->Release();
    }

    void DXDevice::DestroyShader(uint32_t handle)
    {
        IUnknown* obj = (IUnknown*)(size_t)handle;
        if (obj) obj->Release();
    }

} // namespace USE