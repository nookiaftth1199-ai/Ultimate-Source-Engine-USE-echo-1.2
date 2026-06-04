// ============================================================
// Ultimate Source Engine - Render System Implementation
// ============================================================

#include "stdafx.h"
#include "RenderSystem.h"
#include "Core/Window.h"
#include "Core/Logger.h"
#include "OpenGL/GLDevice.h"
#include "DeferredRenderer.h"
#include "ClusteredForwardRenderer.h"
#include "LightCulling.h"

namespace USE
{
	RenderSystem::RenderSystem() = default;
	RenderSystem::~RenderSystem() { Shutdown(); }

	bool RenderSystem::Initialize(RenderBackend backend, Window* window, bool vsync)
	{
		if (!window || !window->IsValid())
		{
			USE_LOG_ERROR("RenderSystem: Invalid window.");
			return false;
		}

		void* nativeHandle = window->GetNativeHandle();
		m_backBufferWidth = window->GetWidth();
		m_backBufferHeight = window->GetHeight();
		m_vsync = vsync;

		// Create the GPU device
		if (backend == RenderBackend::OpenGL || backend == RenderBackend::AutoDetect)
			m_device = std::make_unique<GLDevice>();
		else
		{
			USE_LOG_ERROR("RenderSystem: Only OpenGL backend is currently implemented.");
			return false;
		}

		if (!m_device->Initialize(nativeHandle, m_backBufferWidth, m_backBufferHeight, vsync))
			return false;

		// Create main render target and depth/stencil
		m_backBufferRT = m_device->CreateRenderTarget(m_backBufferWidth, m_backBufferHeight,
			TextureFormat::R8G8B8A8_UNORM, true);
		m_backBufferDS = m_device->CreateDepthStencil(m_backBufferWidth, m_backBufferHeight,
			TextureFormat::D24S8);

		// Initialise the light culling system (for clustered forward)
		m_lightCulling = std::make_unique<LightCulling>();
		m_lightCulling->Initialize(m_backBufferWidth, m_backBufferHeight, 32);

		USE_LOG_INFO("RenderSystem initialized (%u x %u).", m_backBufferWidth, m_backBufferHeight);
		return true;
	}

	void RenderSystem::Shutdown()
	{
		if (m_device)
		{
			if (m_backBufferRT) m_device->DestroyTexture(m_backBufferRT);
			if (m_backBufferDS) m_device->DestroyTexture(m_backBufferDS);
			m_device->Shutdown();
		}
	}

	void RenderSystem::BeginFrame()
	{
		m_device->BeginFrame();
		m_device->SetRenderTarget(m_backBufferRT, m_backBufferDS);
		m_stats.Reset();
	}

	void RenderSystem::EndFrame()
	{
		m_device->EndFrame();
	}

	void RenderSystem::Present()
	{
		m_device->SetRenderTarget(0);  // backbuffer
		m_device->Present();
	}

	void RenderSystem::Clear(int flags, const Color& color, float depth, uint8_t stencil)
	{
		Vector4 clearColor(color.x, color.y, color.z, color.w);
		m_device->Clear((flags & CLEAR_COLOR) != 0,
			(flags & CLEAR_DEPTH) != 0,
			false,
			clearColor, depth, stencil);
	}

	void RenderSystem::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_device->SetViewport(x, y, width, height);
	}

	void RenderSystem::RenderScene(const Scene* scene, const Camera* camera)
	{
		switch (m_renderingMode)
		{
		case RenderingMode::Deferred:
			if (m_deferredRenderer)
				m_deferredRenderer->Render(scene, camera);
			break;
		case RenderingMode::ClusteredForward:
			if (m_clusteredForward)
				m_clusteredForward->Render(scene, camera);
			break;
		default:
			break;
		}
	}

	void RenderSystem::RenderUI()
	{
		// Called by editor – placeholder.
	}

	uint32_t RenderSystem::GetWidth() const { return m_backBufferWidth; }
	uint32_t RenderSystem::GetHeight() const { return m_backBufferHeight; }
}