// ============================================================
// Ultimate Source Engine - Deferred Renderer Implementation
// ============================================================

#include "stdafx.h"
#include "DeferredRenderer.h"
#include "IRenderDevice.h"
#include "LightCulling.h"
#include "Scene/Scene.h"
#include "Entity/Entity.h"
#include "Entity/Components/RenderComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Core/Logger.h"

namespace USE
{
	DeferredRenderer::DeferredRenderer() = default;
	DeferredRenderer::~DeferredRenderer() { Shutdown(); }

	bool DeferredRenderer::Initialize(IRenderDevice* device, uint32_t width, uint32_t height)
	{
		m_device = device;
		m_width = width;
		m_height = height;

		// Create G‑buffer textures
		m_gbufferAlbedo = device->CreateRenderTarget(width, height, TextureFormat::R8G8B8A8_UNORM, false);
		m_gbufferNormal = device->CreateRenderTarget(width, height, TextureFormat::R32G32B32A32_FLOAT, false);
		m_gbufferRoughness = device->CreateRenderTarget(width, height, TextureFormat::R8G8B8A8_UNORM, false);
		m_gbufferDepth = device->CreateDepthStencil(width, height, TextureFormat::D24S8);

		if (!m_gbufferAlbedo || !m_gbufferNormal || !m_gbufferRoughness || !m_gbufferDepth)
		{
			USE_LOG_ERROR("DeferredRenderer: Failed to create G‑buffer.");
			return false;
		}

		// Create a full‑screen quad vertex buffer (to be used in lighting pass)
		struct FVertex { float x, y, z, u, v; };
		FVertex quad[] = {
			{-1, 1,0, 0,0}, {1,1,0, 1,0}, {-1,-1,0, 0,1}, {1,-1,0, 1,1}
		};
		m_fullscreenVB = device->CreateVertexBuffer(quad, sizeof(quad), BufferUsage::Static);

		// Shader programs (placeholder – real shaders would be loaded from files)
		// m_geometryProgram = LoadShader("gbuffer.vert", "gbuffer.frag");
		// m_lightingProgram = LoadShader("deferred_lighting.vert", "deferred_lighting.frag");

		return true;
	}

	void DeferredRenderer::Shutdown()
	{
		if (m_device)
		{
			if (m_gbufferAlbedo)    m_device->DestroyTexture(m_gbufferAlbedo);
			if (m_gbufferNormal)    m_device->DestroyTexture(m_gbufferNormal);
			if (m_gbufferRoughness) m_device->DestroyTexture(m_gbufferRoughness);
			if (m_gbufferDepth)     m_device->DestroyTexture(m_gbufferDepth);
			if (m_fullscreenVB)     m_device->DestroyBuffer(m_fullscreenVB);
		}
	}

	void DeferredRenderer::Render(const Scene* scene, const Camera* camera)
	{
		if (!m_device || !scene || !camera) return;

		// 1. Geometry pass – render scene into G‑buffer
		// For now, iterate entities and draw with geometry shader (stub)
		const auto& entities = scene->GetEntities();
		for (Entity* ent : entities)
		{
			auto renderComp = ent->GetComponent<RenderComponent>();
			auto transform = ent->GetComponent<TransformComponent>();
			if (!renderComp || !transform) continue;

			Model* model = renderComp->GetModel();
			if (!model) continue;

			for (uint32_t m = 0; m < model->GetMeshCount(); ++m)
			{
				Mesh& mesh = model->GetMesh(m);
				m_device->SetVertexBuffer(mesh.GetVertexBuffer(), 0, mesh.GetVertexStride());
				m_device->SetIndexBuffer(mesh.GetIndexBuffer(), mesh.IsIndexBuffer32Bit());
				// Set constant buffers for world, view, proj...
				m_device->DrawIndexed(PrimitiveType::Triangles, mesh.GetIndexCount());
			}
		}

		// 2. Lighting pass – fullscreen quad that samples G‑buffer and accumulates light
		// m_device->SetProgram(m_lightingProgram);
		m_device->SetVertexBuffer(m_fullscreenVB, 0, sizeof(float) * 5);
		m_device->SetTexture(m_gbufferAlbedo, 0);
		m_device->SetTexture(m_gbufferNormal, 1);
		m_device->SetTexture(m_gbufferRoughness, 2);
		m_device->SetTexture(m_gbufferDepth, 3);
		// (light culling data would be bound here)
		m_device->Draw(PrimitiveType::TriangleStrip, 4);
	}
}