// ============================================================
// Ultimate Source Engine - DirectX 9 Vertex Declaration
// ============================================================
//
// Manages a Direct3D 9 vertex declaration, which describes the
// layout of vertex data for use with vertex shaders.
// ============================================================

#pragma once

#include "stdafx.h"
#include <d3d9.h>
#include <vector>

namespace USE {

    // Vertex element description (same as in RenderTypes.h, but we'll define it here to avoid dependency)
    struct VertexElement {
        uint16_t        stream;      // input slot (usually 0)
        uint16_t        offset;      // offset in bytes from start of vertex
        D3DDECLTYPE     type;        // DirectX type (e.g., D3DDECLTYPE_FLOAT3)
        D3DDECLUSAGE    usage;       // e.g., D3DDECLUSAGE_POSITION
        uint8_t         usageIndex;  // index for multiple usages (e.g., TEXCOORD0, TEXCOORD1)
    };

    class DXVertexDeclaration {
    public:
        DXVertexDeclaration();
        ~DXVertexDeclaration();

        // Create from an array of VertexElement
        bool Create(IDirect3DDevice9* device, const VertexElement* elements, UINT count);

        // Bind the vertex declaration to the device
        void Bind(IDirect3DDevice9* device);
        void Unbind(IDirect3DDevice9* device);

        // Get the D3D handle
        IDirect3DVertexDeclaration9* GetHandle() const { return m_decl; }

        // Compute vertex stride (total size of one vertex) for a given stream
        UINT GetStreamStride(UINT stream) const;

        // Check if declaration is valid
        bool IsValid() const { return m_decl != nullptr; }

    private:
        IDirect3DVertexDeclaration9* m_decl;
        std::vector<UINT>            m_streamStrides; // stride per stream

        void Release();
    };

} // namespace USE