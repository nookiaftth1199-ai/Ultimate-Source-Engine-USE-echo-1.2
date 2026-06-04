// ============================================================
// Ultimate Source Engine - Deferred Renderer
// ============================================================
// Renders the scene into a G‑Buffer, then applies lighting
// using the clustered light culling system.
// ============================================================

#pragma once

#include "IRenderDevice.h"
#include "Math/Matrix4.h"
#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include <vector>

namespace USE
{
	class Scene;
	class LightCulling;

	class DeferredRenderer
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		bool Initialize(IRenderDevice* device, uint32_t width, uint32_t height);
		void Shutdown();

		// Render the scene from the given camera’s point of view.
		void RenderScene(IRenderDevice* device, const Scene* scene, const Camera* camera);

		// Access the G‑Buffer textures (for debugging or post‑processing).
		uint32_t GetAlbedoTexture()   const { return m_gbufferAlbedo; }
		uint32_t GetNormalTexture()   const { return m_gbufferNormal; }
		uint32_t GetRoughnessTexture() const { return m_gbufferRoughness; }
		uint32_t GetDepthTexture()    const { return m_gbufferDepth; }

		// Set the light culling system (normally owned by RenderSystem).
		void SetLightCulling(LightCulling* lightCulling) { m_lightCulling = lightCulling; }

	private:
		bool CreateGBuffer();
		bool CreateShaders();
		bool CreateFullscreenQuad();

		void GeometryPass(IRenderDevice* device, const Scene* scene, const Camera* camera);
		void LightingPass(IRenderDevice* device, const Scene* scene, const Camera* camera);

		IRenderDevice* m_device = nullptr;
		LightCulling*  m_lightCulling = nullptr;

		// G‑Buffer
		uint32_t m_gbufferAlbedo = 0;
		uint32_t m_gbufferNormal = 0;
		uint32_t m_gbufferRoughness = 0;
		uint32_t m_gbufferDepth = 0;
		uint32_t m_gbufferFBO = 0;   // framebuffer object (if using OpenGL-style)

		uint32_t m_width = 0;
		uint32_t m_height = 0;

		// Shader programs
		uint32_t m_geometryProgram = 0;   // writes to G‑Buffer
		uint32_t m_lightingProgram = 0;   // deferred lighting

		// Full‑screen quad for lighting pass
		uint32_t m_fullscreenVB = 0;

		bool m_initialized = false;
	};
}