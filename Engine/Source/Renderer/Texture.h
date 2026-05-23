// ============================================================
// Ultimate Source Engine - Texture
// ============================================================
//
// Represents a 2D texture. Supports loading from files,
// creating from memory, and dynamic updates (e.g., for video playback).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/RenderTypes.h"
#include "Math/Color.h"

namespace USE {

    class IRenderDevice;

    class Texture {
    public:
        Texture();
        ~Texture();

        // Load texture from file (PNG, JPG, TGA, etc. via stb_image)
        bool LoadFromFile(const char* filename);

        // Create an empty texture with given size and format (optional initial data)
        bool Create(int width, int height, TextureFormat format, const void* data = nullptr);

        // Update texture data (for dynamic textures like video frames)
        bool Update(const void* data, int width, int height, TextureFormat format);

        // Bind/unbind texture to a specific texture unit
        void Bind(int unit = 0);
        void Unbind(int unit = 0);

        // Set texture parameters
        void SetWrap(TextureWrap s, TextureWrap t);
        void SetFilter(TextureFilter min, TextureFilter mag);
        void SetBorderColor(const Color& color);

        // Generate mipmaps (if supported)
        void GenerateMipmaps();

        // Query texture info
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        TextureFormat GetFormat() const { return m_format; }
        unsigned int GetHandle() const { return m_handle; }
        bool IsValid() const { return m_handle != 0; }

        // Static factory: creates a texture for the current render backend
        static Texture* Create();

    private:
        unsigned int    m_handle;   // Backend‑specific texture handle (OpenGL: GLuint, etc.)
        int             m_width;
        int             m_height;
        TextureFormat   m_format;
        IRenderDevice*  m_device;   // optional, for backend operations
    };

} // namespace USE