// ============================================================
// Ultimate Source Engine – DirectX 9 Render Device Implementation
// ============================================================

#include "stdafx.h"
#include "DXDevice.h"
#include "Core/Logger.h"
#include <SDL_syswm.h>   // for SDL_GetWindowWMInfo

namespace USE
{
	DXDevice::DXDevice() = default;
	DXDevice::~DXDevice() { Shutdown(); }

	bool DXDevice::Initialize(void* windowHandle, uint32_t width, uint32_t height, bool vsync)
	{
		SDL_Window* window = static_cast<SDL_Window*>(windowHandle);
		if (!window)
		{
			USE_LOG_ERROR("DXDevice: Invalid window handle.");
			return false;
		}

		m_width = width;
		m_height = height;

		// Get native HWND
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (!SDL_GetWindowWMInfo(window, &wmInfo))
		{
			USE_LOG_ERROR("DXDevice: Failed to get native window info: %s", SDL_GetError());
			return false;
		}
		m_hwnd = wmInfo.info.win.window;

		// Create Direct3D9 object
		m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (!m_d3d)
		{
			USE_LOG_ERROR("DXDevice: Direct3DCreate9 failed.");
			return false;
		}

		ZeroMemory(&m_presentParams, sizeof(m_presentParams));
		m_presentParams.Windowed = TRUE;
		m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_presentParams.BackBufferFormat = D3DFMT_UNKNOWN;
		m_presentParams.BackBufferWidth = width;
		m_presentParams.BackBufferHeight = height;
		m_presentParams.EnableAutoDepthStencil = TRUE;
		m_presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_presentParams.PresentationInterval = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		HRESULT hr = m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_presentParams, &m_device);
		if (FAILED(hr))
		{
			USE_LOG_ERROR("DXDevice: CreateDevice failed (0x%08X).", hr);
			return false;
		}

		// Default render states
		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		m_device->SetRenderState(D3DRS_LIGHTING, FALSE);

		m_initialized = true;
		USE_LOG_INFO("DXDevice initialized (%u x %u).", width, height);
		return true;
	}

	void DXDevice::Shutdown()
	{
		if (m_device) { m_device->Release(); m_device = nullptr; }
		if (m_d3d) { m_d3d->Release();    m_d3d = nullptr; }
		m_initialized = false;
		USE_LOG_INFO("DXDevice shut down.");
	}

	void DXDevice::ResizeBackBuffer(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		m_presentParams.BackBufferWidth = width;
		m_presentParams.BackBufferHeight = height;
		if (m_device)
			m_device->Reset(&m_presentParams);
	}

	void DXDevice::BeginFrame() {}
	void DXDevice::EndFrame() {}
	void DXDevice::Present() { if (m_device) m_device->Present(nullptr, nullptr, nullptr, nullptr); }

	void DXDevice::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		D3DVIEWPORT9 vp = { x, y, w, h, 0.0f, 1.0f };
		if (m_device) m_device->SetViewport(&vp);
	}

	void DXDevice::SetScissorRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		RECT rect = { (LONG)x, (LONG)y, (LONG)(x + w), (LONG)(y + h) };
		if (m_device) m_device->SetScissorRect(&rect);
	}

	void DXDevice::SetDepthStencilState(bool depthTest, bool depthWrite)
	{
		if (!m_device) return;
		m_device->SetRenderState(D3DRS_ZENABLE, depthTest ? D3DZB_TRUE : D3DZB_FALSE);
		m_device->SetRenderState(D3DRS_ZWRITEENABLE, depthWrite ? TRUE : FALSE);
	}

	void DXDevice::SetRasterizerState(bool cullBackFaces, bool wireframe)
	{
		if (!m_device) return;
		m_device->SetRenderState(D3DRS_CULLMODE, cullBackFaces ? D3DCULL_CCW : D3DCULL_NONE);
		m_device->SetRenderState(D3DRS_FILLMODE, wireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	}

	void DXDevice::SetBlendState(bool enable)
	{
		if (!m_device) return;
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
	}

	void DXDevice::Clear(bool color, bool depth, bool stencil,
		const Vector4& clearColor, float clearDepth, uint8_t clearStencil)
	{
		if (!m_device) return;
		DWORD flags = 0;
		if (color) flags |= D3DCLEAR_TARGET;
		if (depth) flags |= D3DCLEAR_ZBUFFER;
		if (stencil) flags |= D3DCLEAR_STENCIL;
		m_device->Clear(0, nullptr, flags,
			D3DCOLOR_COLORVALUE(clearColor.x, clearColor.y, clearColor.z, clearColor.w),
			clearDepth, clearStencil);
	}

	void DXDevice::Draw(PrimitiveType type, uint32_t vertexCount, uint32_t startVertex)
	{
		if (!m_device) return;
		// Requires vertex buffer to be set, so placeholder.
	}

	void DXDevice::DrawIndexed(PrimitiveType type, uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
	{
		if (!m_device) return;
	}
}