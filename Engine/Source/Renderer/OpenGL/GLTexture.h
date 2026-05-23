// ============================================================
// Ultimate Source Engine - OpenGL Texture
// ============================================================
//
// Manages OpenGL 2D textures. Supports loading from files,
// creating blank textures, setting parameters, and binding.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Renderer/RenderTypes.h"

namespace USE {

    class GLTexture {
    public:
        GLTexture();
        ~GLTexture();

        // Load texture from file (supports PNG, JPG, TGA, etc. via stb_image)
        bool LoadFromFile(const char* filename);

        // Create an empty texture of given size/format (can upload data later)
        bool Create(int width, int height, TextureFormat format, const void* data = nullptr);

        // Bind/unbind texture to a specific texture unit
        void Bind(int unit = 0);
        void Unbind(int unit = 0);

        // Set texture parameters
        void SetWrap(TextureWrap s, TextureWrap t);
        void SetFilter(TextureFilter min, TextureFilter mag);
        void SetBorderColor(const Color& color);

        // Generate mipmaps (if supported)
        void GenerateMipmaps();

        // Get texture info
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        TextureFormat GetFormat() const { return m_format; }
        unsigned int GetHandle() const { return m_handle; }
        bool IsValid() const { return m_handle != 0; }

    private:
        unsigned int    m_handle;      // OpenGL texture object
        int             m_width;
        int             m_height;
        TextureFormat   m_format;

        // Convert TextureFormat to OpenGL internal format and pixel format
        void GetGLFormats(TextureFormat format, GLint& internalFormat, GLenum& pixelFormat, GLenum& pixelType) const;
    };

} // namespace USE