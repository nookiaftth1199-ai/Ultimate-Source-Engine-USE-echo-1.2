// ============================================================
// Ultimate Source Engine - DirectX 9 Vertex Declaration Implementation
// ============================================================

#include "stdafx.h"
#include "DXVertexDeclaration.h"
#include "Core/Logger.h"

namespace USE {

    DXVertexDeclaration::DXVertexDeclaration()
        : m_decl(nullptr)
    {
    }

    DXVertexDeclaration::~DXVertexDeclaration()
    {
        Release();
    }

    void DXVertexDeclaration::Release()
    {
        if (m_decl) {
            m_decl->Release();
            m_decl = nullptr;
        }
        m_streamStrides.clear();
    }

    bool DXVertexDeclaration::Create(IDirect3DDevice9* device, const VertexElement* elements, UINT count)
    {
        if (!device) {
            USE_LOG_ERROR("DXVertexDeclaration::Create: device is null");
            return false;
        }
        if (!elements || count == 0) {
            USE_LOG_ERROR("DXVertexDeclaration::Create: no elements provided");
            return false;
        }

        Release();

        // Convert to D3DVERTEXELEMENT9 array
        std::vector<D3DVERTEXELEMENT9> d3dElements;
        d3dElements.reserve(count + 1); // +1 for D3DDECL_END

        // Track max offset per stream to compute stride later
        std::vector<UINT> maxOffsets; // stream -> max offset

        for (UINT i = 0; i < count; ++i) {
            const VertexElement& elem = elements[i];
            D3DVERTEXELEMENT9 d3dElem;
            d3dElem.Stream = elem.stream;
            d3dElem.Offset = elem.offset;
            d3dElem.Type = elem.type;
            d3dElem.Method = D3DDECLMETHOD_DEFAULT;
            d3dElem.Usage = elem.usage;
            d3dElem.UsageIndex = elem.usageIndex;

            d3dElements.push_back(d3dElem);

            // Update max offset for this stream
            if (elem.stream >= maxOffsets.size()) {
                maxOffsets.resize(elem.stream + 1, 0);
            }
            UINT elementSize = 0;
            switch (elem.type) {
                case D3DDECLTYPE_FLOAT1:   elementSize = 4; break;
                case D3DDECLTYPE_FLOAT2:   elementSize = 8; break;
                case D3DDECLTYPE_FLOAT3:   elementSize = 12; break;
                case D3DDECLTYPE_FLOAT4:   elementSize = 16; break;
                case D3DDECLTYPE_D3DCOLOR: elementSize = 4; break;
                case D3DDECLTYPE_UBYTE4:   elementSize = 4; break;
                case D3DDECLTYPE_SHORT2:   elementSize = 4; break;
                case D3DDECLTYPE_SHORT4:   elementSize = 8; break;
                case D3DDECLTYPE_UBYTE4N:  elementSize = 4; break;
                case D3DDECLTYPE_SHORT2N:  elementSize = 4; break;
                case D3DDECLTYPE_SHORT4N:  elementSize = 8; break;
                case D3DDECLTYPE_USHORT2N: elementSize = 4; break;
                case D3DDECLTYPE_USHORT4N: elementSize = 8; break;
                case D3DDECLTYPE_UDEC3:    elementSize = 4; break;
                case D3DDECLTYPE_DEC3N:    elementSize = 4; break;
                case D3DDECLTYPE_FLOAT16_2: elementSize = 4; break;
                case D3DDECLTYPE_FLOAT16_4: elementSize = 8; break;
                default: elementSize = 0; break;
            }
            UINT endOffset = elem.offset + elementSize;
            if (endOffset > maxOffsets[elem.stream]) {
                maxOffsets[elem.stream] = endOffset;
            }
        }

        // Add end marker
        D3DVERTEXELEMENT9 end = D3DDECL_END();
        d3dElements.push_back(end);

        // Create the vertex declaration
        HRESULT hr = device->CreateVertexDeclaration(d3dElements.data(), &m_decl);
        if (FAILED(hr)) {
            USE_LOG_ERROR("DXVertexDeclaration::Create: failed (hr=0x%08X)", hr);
            return false;
        }

        // Store strides per stream
        m_streamStrides.resize(maxOffsets.size());
        for (size_t i = 0; i < maxOffsets.size(); ++i) {
            m_streamStrides[i] = maxOffsets[i];
        }

        return true;
    }

    void DXVertexDeclaration::Bind(IDirect3DDevice9* device)
    {
        if (m_decl && device) {
            device->SetVertexDeclaration(m_decl);
        }
    }

    void DXVertexDeclaration::Unbind(IDirect3DDevice9* device)
    {
        if (device) {
            device->SetVertexDeclaration(nullptr);
        }
    }

    UINT DXVertexDeclaration::GetStreamStride(UINT stream) const
    {
        if (stream < m_streamStrides.size()) {
            return m_streamStrides[stream];
        }
        return 0;
    }

} // namespace USE