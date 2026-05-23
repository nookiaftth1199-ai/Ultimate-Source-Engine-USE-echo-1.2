// ============================================================
// Ultimate Source Engine - DirectX 9 Buffer
// ============================================================
//
// Encapsulates DirectX 9 vertex and index buffers.
// Supports creation, locking, updating, and binding.
// ============================================================

#pragma once

#include "stdafx.h"
#include <d3d9.h>

namespace USE {

    enum class DXBufferType {
        Vertex,
        Index
    };

    enum class DXBufferUsage {
        Static,   // D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED
        Dynamic,  // D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT
        Staging   // D3DPOOL_SYSTEMMEM (for readback)
    };

    class DXBuffer {
    public:
        DXBuffer();
        ~DXBuffer();

        // Create a buffer of given type and size, optionally with initial data
        bool Create(IDirect3DDevice9* device, DXBufferType type, size_t size,
                    DXBufferUsage usage = DXBufferUsage::Static, const void* data = nullptr);

        // Map buffer for CPU access (write only, optionally discard if dynamic)
        void* Lock(size_t offset = 0, size_t size = 0, bool discard = false);

        // Unmap buffer after lock
        void Unlock();

        // Bind the buffer to the device (for rendering)
        void Bind(IDirect3DDevice9* device, UINT streamNumber = 0, UINT stride = 0, UINT offset = 0);

        // Unbind (optional, resets stream source)
        void Unbind(IDirect3DDevice9* device, UINT streamNumber = 0);

        // Get buffer info
        DXBufferType GetType() const { return m_type; }
        size_t GetSize() const { return m_size; }
        bool IsValid() const { return m_buffer != nullptr; }

        // DirectX handles
        IDirect3DVertexBuffer9* GetVertexBuffer() const {
            return (m_type == DXBufferType::Vertex) ? (IDirect3DVertexBuffer9*)m_buffer : nullptr;
        }
        IDirect3DIndexBuffer9* GetIndexBuffer() const {
            return (m_type == DXBufferType::Index) ? (IDirect3DIndexBuffer9*)m_buffer : nullptr;
        }

    private:
        IUnknown* m_buffer;   // Vertex or index buffer interface
        DXBufferType m_type;
        size_t      m_size;

        void Release();
    };

} // namespace USE