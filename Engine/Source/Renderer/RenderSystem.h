// ============================================================
// Ultimate Source Engine - Render System
// ============================================================
// High‑level rendering manager. Owns the concrete render device
// and orchestrates all drawing passes.
// ============================================================

#pragma once

#include "IRenderDevice.h"
#include "RenderStats.h"
#include "RenderTypes.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4.h"
#include <memory>
#include <string>
#include <vector>

namespace USE
{
	class Window;
	class Camera;
	class Scene;

	enum class RenderBackend
	{
		AutoDetect,
		OpenGL,
		DirectX9,
		Vulkan
	};

	enum class RenderingMode
	{
		Forward,
		Deferred,
		ClusteredForward
	};

	class RenderSystem
	{
	public:
		RenderSystem();
		~RenderSystem();

		// Initialization & shutdown
		bool Initialize(RenderBackend backend, Window* window, bool vsync);
		void Shutdown();

		// Frame lifecycle
		void BeginFrame();
		void EndFrame();
		void Present();

		// Core rendering commands
		void Clear(int flags, const Color& color, float depth = 1.0f, uint8_t stencil = 0);
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		// High‑level rendering passes
		void RenderScene(const Scene* scene, const Camera* camera);
		void RenderUI();

		// Feature control
		void SetRenderingMode(RenderingMode mode) { m_renderingMode = mode; }
		RenderingMode GetRenderingMode() const { return m_renderingMode; }

		// Access to device
		IRenderDevice* GetDevice() const { return m_device.get(); }
		RenderStats& GetStats() { return m_stats; }

		// Back‑buffer info
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		bool CreateDevice(RenderBackend backend, void* windowHandle, bool vsync);
		void DestroyDevice();
		void RecreateSizeDependentResources();

		RenderBackend DetectBackend() const;
		RenderBackend m_requestedBackend = RenderBackend::AutoDetect;

		std::unique_ptr<IRenderDevice> m_device;

		// Rendering mode
		RenderingMode m_renderingMode = RenderingMode::Deferred;

		// Resources
		uint32_t m_backBufferRT = 0;
		uint32_t m_backBufferDS = 0;
		uint32_t m_backBufferWidth = 0;
		uint32_t m_backBufferHeight = 0;
		bool m_vsync = false;

		RenderStats m_stats;
	};
}