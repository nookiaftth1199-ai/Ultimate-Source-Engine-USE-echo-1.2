// ============================================================
// Ultimate Source Engine - Mesh
// ============================================================
//
// Represents a 3D mesh with vertex and index buffers.
// Provides methods to create from CPU data and bind for rendering.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/IRenderDevice.h"

namespace USE {

    class Mesh {
    public:
        Mesh();
        ~Mesh();

        // Create from raw data (CPU memory). Pass device to create GPU buffers.
        bool Create(IRenderDevice* device,
                    const void* vertices, uint32_t vertexCount, uint32_t vertexStride,
                    const uint32_t* indices, uint32_t indexCount);

        // Destroy GPU resources
        void Destroy();

        // Bind buffers for rendering (sets vertex and index buffers in device)
        void Bind(IRenderDevice* device) const;

        // Draw the mesh (assumes buffers are bound)
        void Draw(IRenderDevice* device) const;

        // Getters
        uint32_t GetVertexCount() const   { return m_vertexCount; }
        uint32_t GetIndexCount() const    { return m_indexCount; }
        uint32_t GetVertexStride() const  { return m_vertexStride; }
        bool     IsValid() const          { return m_vertexBuffer != 0; }

    private:
        uint32_t m_vertexBuffer;
        uint32_t m_indexBuffer;
        uint32_t m_vertexCount;
        uint32_t m_indexCount;
        uint32_t m_vertexStride;
    };

} // namespace USE