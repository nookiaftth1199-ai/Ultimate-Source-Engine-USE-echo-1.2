// ============================================================
// Ultimate Source Engine - DirectX 9 Buffer Implementation
// ============================================================

#include "stdafx.h"
#include "DXBuffer.h"
#include "Core/Logger.h"

namespace USE {

    DXBuffer::DXBuffer()
        : m_buffer(nullptr)
        , m_type(DXBufferType::Vertex)
        , m_size(0)
    {
    }

    DXBuffer::~DXBuffer()
    {
        Release();
    }

    void DXBuffer::Release()
    {
        if (m_buffer) {
            m_buffer->Release();
            m_buffer = nullptr;
        }
        m_size = 0;
    }

    bool DXBuffer::Create(IDirect3DDevice9* device, DXBufferType type, size_t size,
                          DXBufferUsage usage, const void* data)
    {
        if (!device) {
            USE_LOG_ERROR("DXBuffer::Create: device is null");
            return false;
        }
        if (size == 0) {
            USE_LOG_ERROR("DXBuffer::Create: size cannot be zero");
            return false;
        }

        Release();

        DWORD d3dUsage = 0;
        D3DPOOL pool = D3DPOOL_MANAGED;

        switch (usage) {
            case DXBufferUsage::Static:
                d3dUsage = 0; // no flags
                pool = D3DPOOL_MANAGED;
                break;
            case DXBufferUsage::Dynamic:
                d3dUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
                pool = D3DPOOL_DEFAULT;
                break;
            case DXBufferUsage::Staging:
                d3dUsage = 0;
                pool = D3DPOOL_SYSTEMMEM;
                break;
        }

        HRESULT hr;
        if (type == DXBufferType::Vertex) {
            IDirect3DVertexBuffer9* vb = nullptr;
            hr = device->CreateVertexBuffer(
                (UINT)size,
                d3dUsage,
                0,                    // FVF (not used with shaders)
                pool,
                &vb,
                nullptr);
            if (SUCCEEDED(hr)) {
                m_buffer = vb;
            }
        } else { // Index buffer
            IDirect3DIndexBuffer9* ib = nullptr;
            hr = device->CreateIndexBuffer(
                (UINT)size,
                d3dUsage,
                D3DFMT_INDEX32,        // 32‑bit indices (could be 16‑bit if needed)
                pool,
                &ib,
                nullptr);
            if (SUCCEEDED(hr)) {
                m_buffer = ib;
            }
        }

        if (FAILED(hr)) {
            USE_LOG_ERROR("DXBuffer::Create: failed to create buffer (hr=0x%08X)", hr);
            return false;
        }

        m_type = type;
        m_size = size;

        // Upload initial data if provided
        if (data) {
            void* ptr = Lock(0, size, false);
            if (ptr) {
                memcpy(ptr, data, size);
                Unlock();
            } else {
                USE_LOG_ERROR("DXBuffer::Create: failed to lock for initial data");
                Release();
                return false;
            }
        }

        return true;
    }

    void* DXBuffer::Lock(size_t offset, size_t size, bool discard)
    {
        if (!m_buffer) return nullptr;

        if (size == 0) size = m_size;
        if (offset + size > m_size) return nullptr;

        DWORD flags = 0;
        if (discard) flags |= D3DLOCK_DISCARD;

        void* ptr = nullptr;
        HRESULT hr;

        if (m_type == DXBufferType::Vertex) {
            hr = ((IDirect3DVertexBuffer9*)m_buffer)->Lock(
                (UINT)offset, (UINT)size, &ptr, flags);
        } else {
            hr = ((IDirect3DIndexBuffer9*)m_buffer)->Lock(
                (UINT)offset, (UINT)size, &ptr, flags);
        }

        if (FAILED(hr)) {
            USE_LOG_ERROR("DXBuffer::Lock failed (hr=0x%08X)", hr);
            return nullptr;
        }

        return ptr;
    }

    void DXBuffer::Unlock()
    {
        if (!m_buffer) return;
        if (m_type == DXBufferType::Vertex) {
            ((IDirect3DVertexBuffer9*)m_buffer)->Unlock();
        } else {
            ((IDirect3DIndexBuffer9*)m_buffer)->Unlock();
        }
    }

    void DXBuffer::Bind(IDirect3DDevice9* device, UINT streamNumber, UINT stride, UINT offset)
    {
        if (!m_buffer || !device) return;
        if (m_type == DXBufferType::Vertex) {
            device->SetStreamSource(streamNumber, (IDirect3DVertexBuffer9*)m_buffer, offset, stride);
        } else {
            device->SetIndices((IDirect3DIndexBuffer9*)m_buffer);
        }
    }

    void DXBuffer::Unbind(IDirect3DDevice9* device, UINT streamNumber)
    {
        if (!device) return;
        if (m_type == DXBufferType::Vertex) {
            device->SetStreamSource(streamNumber, nullptr, 0, 0);
        } else {
            device->SetIndices(nullptr);
        }
    }

} // namespace USE