// ============================================================
// Ultimate Source Engine - DirectX 9 Texture
// ============================================================
//
// Manages DirectX 9 2D textures. Supports loading from files,
// creating blank textures, setting parameters, and binding.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Renderer/RenderTypes.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace USE {

    class DXTexture {
    public:
        DXTexture();
        ~DXTexture();

        // Load texture from file using D3DX
        bool LoadFromFile(IDirect3DDevice9* device, const char* filename);

        // Create an empty texture of given size/format (can upload data later)
        bool Create(IDirect3DDevice9* device, int width, int height,
                    TextureFormat format, const void* data = nullptr);

        // Bind texture to a specific sampler stage
        void Bind(IDirect3DDevice9* device, int stage = 0);
        void Unbind(IDirect3DDevice9* device, int stage = 0);

        // Set texture state (sampler states)
        void SetWrap(IDirect3DDevice9* device, int stage, TextureWrap s, TextureWrap t);
        void SetFilter(IDirect3DDevice9* device, int stage, TextureFilter min, TextureFilter mag);

        // Get texture info
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        TextureFormat GetFormat() const { return m_format; }
        IDirect3DTexture9* GetHandle() const { return m_texture; }
        bool IsValid() const { return m_texture != nullptr; }

    private:
        IDirect3DTexture9* m_texture;
        int                m_width;
        int                m_height;
        TextureFormat      m_format;

        // Convert TextureFormat to D3DFORMAT
        D3DFORMAT ToD3DFormat(TextureFormat format) const;
    };

} // namespace USE