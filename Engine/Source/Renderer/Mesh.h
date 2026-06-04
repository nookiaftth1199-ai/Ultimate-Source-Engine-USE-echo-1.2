// ============================================================
// Ultimate Source Engine - Mesh
// ============================================================
// Holds vertex/index buffers and a vertex declaration.
// ============================================================

#pragma once

#include "Material.h"
#include "IRenderDevice.h"
#include <vector>
#include <cstdint>

namespace USE
{
	class Material;

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		// Set the raw vertex data and build the GPU buffers.
		void SetVertices(const void* data, uint32_t count, uint32_t stride,
			const std::vector<VertexElement>& layout);

		// Set index data (16‑bit or 32‑bit).
		void SetIndices(const uint32_t* data, uint32_t count);
		void SetIndices(const uint16_t* data, uint32_t count);

		// Material slots (one material per surface).
		void SetMaterial(uint32_t slot, const Material& mat);
		const Material& GetMaterial(uint32_t slot) const;
		uint32_t GetMaterialCount() const { return static_cast<uint32_t>(m_materials.size()); }

		// Accessors for the renderer.
		uint32_t GetVertexBuffer()       const { return m_vertexBuffer; }
		uint32_t GetIndexBuffer()        const { return m_indexBuffer; }
		uint32_t GetVertexDeclaration()  const { return m_vertexDecl; }
		uint32_t GetVertexCount()        const { return m_vertexCount; }
		uint32_t GetIndexCount()         const { return m_indexCount; }
		bool     IsIndexBuffer32Bit()    const { return m_index32Bit; }
		uint32_t GetVertexStride()       const { return m_vertexStride; }

		// Set material indices per surface (used by MapLoader).
		void SetMaterialIndices(const uint32_t* surfaceMaterials, uint32_t surfaceCount);

	private:
		IRenderDevice* m_device = nullptr;

		uint32_t m_vertexBuffer = 0;
		uint32_t m_indexBuffer = 0;
		uint32_t m_vertexDecl = 0;
		uint32_t m_vertexCount = 0;
		uint32_t m_indexCount = 0;
		uint32_t m_vertexStride = 0;
		bool     m_index32Bit = true;

		std::vector<Material> m_materials;
	};
}