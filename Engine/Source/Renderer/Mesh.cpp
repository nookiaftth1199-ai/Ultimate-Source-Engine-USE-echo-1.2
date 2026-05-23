// ============================================================
// Ultimate Source Engine - Mesh Implementation
// ============================================================

#include "stdafx.h"
#include "Mesh.h"
#include "Core/Logger.h"

namespace USE {

    Mesh::Mesh()
        : m_vertexBuffer(0)
        , m_indexBuffer(0)
        , m_vertexCount(0)
        , m_indexCount(0)
        , m_vertexStride(0)
    {
    }

    Mesh::~Mesh()
    {
        Destroy();
    }

    bool Mesh::Create(IRenderDevice* device,
                      const void* vertices, uint32_t vertexCount, uint32_t vertexStride,
                      const uint32_t* indices, uint32_t indexCount)
    {
        if (!device) {
            USE_LOG_ERROR("Mesh::Create: device is null");
            return false;
        }
        if (!vertices || vertexCount == 0 || vertexStride == 0) {
            USE_LOG_ERROR("Mesh::Create: invalid vertex data");
            return false;
        }
        if (indices && indexCount == 0) {
            USE_LOG_ERROR("Mesh::Create: index buffer with zero count");
            return false;
        }

        // Destroy any existing buffers
        Destroy();

        size_t vertexBufferSize = vertexCount * vertexStride;
        if (!device->CreateVertexBuffer(vertices, vertexBufferSize, m_vertexBuffer)) {
            USE_LOG_ERROR("Mesh::Create: failed to create vertex buffer");
            return false;
        }

        if (indices) {
            size_t indexBufferSize = indexCount * sizeof(uint32_t);
            if (!device->CreateIndexBuffer(indices, indexBufferSize, m_indexBuffer)) {
                USE_LOG_ERROR("Mesh::Create: failed to create index buffer");
                device->DestroyBuffer(m_vertexBuffer);
                m_vertexBuffer = 0;
                return false;
            }
        }

        m_vertexCount = vertexCount;
        m_indexCount = indexCount;
        m_vertexStride = vertexStride;

        USE_LOG_INFO("Mesh created: %u vertices, %u indices, stride %u",
                     vertexCount, indexCount, vertexStride);
        return true;
    }

    void Mesh::Destroy()
    {
        if (m_vertexBuffer) {
            // We need a device to destroy buffers. Since we don't store device, we'll rely on the render system.
            // In practice, buffers are managed by the device, and they will be destroyed when the device shuts down.
            // For proper cleanup, we should have a reference to the device, or the resource manager handles it.
            // To keep it simple, we'll assume the render device's DestroyBuffer is called elsewhere,
            // or we accept that buffers are leaked if not explicitly destroyed via the device.
            // For now, we'll just reset handles.
            USE_LOG_WARN("Mesh::Destroy: buffer handles not released; they should be released via device.");
            m_vertexBuffer = 0;
            m_indexBuffer = 0;
        }
    }

    void Mesh::Bind(IRenderDevice* device) const
    {
        if (!device) {
            USE_LOG_ERROR("Mesh::Bind: device is null");
            return;
        }
        if (m_vertexBuffer) {
            // We need a way to bind vertex buffer. IRenderDevice should have a method like SetVertexBuffer.
            // But our current IRenderDevice does not have such methods. We'll assume we can bind via device using handles.
            // For now, we'll comment out; this will be implemented when IRenderDevice is extended.
            // device->SetVertexBuffer(0, m_vertexBuffer, m_vertexStride, 0);
            // device->SetIndexBuffer(m_indexBuffer, 0);
            USE_LOG_WARN("Mesh::Bind: IRenderDevice does not have binding methods yet.");
        }
    }

    void Mesh::Draw(IRenderDevice* device) const
    {
        if (!device) {
            USE_LOG_ERROR("Mesh::Draw: device is null");
            return;
        }
        if (m_indexCount > 0) {
            device->DrawIndexed(m_indexCount, 0, 0);
        } else {
            // Draw non‑indexed: need a separate method (DrawArrays). Not yet in IRenderDevice.
            USE_LOG_WARN("Mesh::Draw: non‑indexed draw not implemented");
        }
    }

} // namespace USE