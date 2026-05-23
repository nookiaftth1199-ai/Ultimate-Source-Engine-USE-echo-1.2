// ============================================================
// Ultimate Source Engine - OpenGL Texture Implementation
// ============================================================

#include "stdafx.h"
#include "GLTexture.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "ThirdParty/stb_image.h"  // Assume stb_image is available

#include <GL/gl.h>
#include <GL/glu.h>

namespace USE {

    GLTexture::GLTexture()
        : m_handle(0)
        , m_width(0)
        , m_height(0)
        , m_format(TextureFormat::Unknown)
    {
    }

    GLTexture::~GLTexture()
    {
        if (m_handle) {
            glDeleteTextures(1, &m_handle);
        }
    }

    bool GLTexture::LoadFromFile(const char* filename)
    {
        // Use FileSystem to get file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("GLTexture::LoadFromFile: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("Texture file not found: %s", filename);
            return false;
        }

        // Load via stb_image
        int width, height, channels;
        unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &channels, 4); // force RGBA
        if (!data) {
            USE_LOG_ERROR("Failed to load texture: %s (stb_image error)", filename);
            return false;
        }

        // Determine format
        TextureFormat format = TextureFormat::RGBA8_UNORM; // we forced 4 channels

        // Create OpenGL texture
        glGenTextures(1, &m_handle);
        glBindTexture(GL_TEXTURE_2D, m_handle);

        // Set default parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Upload texture data
        GLint internalFormat = GL_RGBA8;
        GLenum pixelFormat = GL_RGBA;
        GLenum pixelType = GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                     pixelFormat, pixelType, data);

        // Generate mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, width, height,
                          pixelFormat, pixelType, data);

        // Store dimensions
        m_width = width;
        m_height = height;
        m_format = format;

        // Free image data
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        USE_LOG_INFO("Loaded texture: %s (%dx%d)", filename, width, height);
        return true;
    }

    bool GLTexture::Create(int width, int height, TextureFormat format, const void* data)
    {
        if (width <= 0 || height <= 0) {
            USE_LOG_ERROR("GLTexture::Create: invalid dimensions (%dx%d)", width, height);
            return false;
        }

        glGenTextures(1, &m_handle);
        glBindTexture(GL_TEXTURE_2D, m_handle);

        // Convert format
        GLint internalFormat;
        GLenum pixelFormat, pixelType;
        GetGLFormats(format, internalFormat, pixelFormat, pixelType);

        // Upload texture data (nullptr for empty)
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                     pixelFormat, pixelType, data);

        // Set default parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_width = width;
        m_height = height;
        m_format = format;

        USE_LOG_INFO("Created texture: %dx%d format %d", width, height, (int)format);
        return true;
    }

    void GLTexture::Bind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_handle);
    }

    void GLTexture::Unbind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::SetWrap(TextureWrap s, TextureWrap t)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);

        GLenum glWrapS = GL_REPEAT, glWrapT = GL_REPEAT;
        switch (s) {
            case TextureWrap::Repeat:          glWrapS = GL_REPEAT; break;
            case TextureWrap::MirroredRepeat:  glWrapS = GL_MIRRORED_REPEAT; break;
            case TextureWrap::ClampToEdge:     glWrapS = GL_CLAMP_TO_EDGE; break;
            case TextureWrap::ClampToBorder:   glWrapS = GL_CLAMP_TO_BORDER; break;
            default: break;
        }
        switch (t) {
            case TextureWrap::Repeat:          glWrapT = GL_REPEAT; break;
            case TextureWrap::MirroredRepeat:  glWrapT = GL_MIRRORED_REPEAT; break;
            case TextureWrap::ClampToEdge:     glWrapT = GL_CLAMP_TO_EDGE; break;
            case TextureWrap::ClampToBorder:   glWrapT = GL_CLAMP_TO_BORDER; break;
            default: break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::SetFilter(TextureFilter min, TextureFilter mag)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);

        GLenum glMin = GL_LINEAR, glMag = GL_LINEAR;
        // Map TextureFilter to OpenGL enums (simplified)
        switch (min) {
            case TextureFilter::Nearest:                glMin = GL_NEAREST; break;
            case TextureFilter::Linear:                 glMin = GL_LINEAR; break;
            case TextureFilter::NearestMipmapNearest:   glMin = GL_NEAREST_MIPMAP_NEAREST; break;
            case TextureFilter::LinearMipmapNearest:    glMin = GL_LINEAR_MIPMAP_NEAREST; break;
            case TextureFilter::NearestMipmapLinear:    glMin = GL_NEAREST_MIPMAP_LINEAR; break;
            case TextureFilter::LinearMipmapLinear:     glMin = GL_LINEAR_MIPMAP_LINEAR; break;
        }
        switch (mag) {
            case TextureFilter::Nearest:                glMag = GL_NEAREST; break;
            case TextureFilter::Linear:                 glMag = GL_LINEAR; break;
            default:                                     glMag = GL_LINEAR; break; // mag can't have mipmap
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMag);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::SetBorderColor(const Color& color)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);
        float border[] = { color.r, color.g, color.b, color.a };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::GenerateMipmaps()
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);
        // For OpenGL 2.0, use gluBuild2DMipmaps or extension.
        // Simplest: use glGenerateMipmap if available (requires GL 3.0+).
        // Fallback to gluBuild2DMipmaps.
        // We'll assume glu is available.
        // But we need to have the texture data. gluBuild2DMipmaps requires the base image again.
        // Instead, we can re‑upload the base level and let GLU generate.
        // However, we don't have the pixel data here. So maybe we skip.
        // For simplicity, we'll just set a flag or do nothing.
        USE_LOG_WARN("GLTexture::GenerateMipmaps not fully implemented");
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::GetGLFormats(TextureFormat format, GLint& internalFormat, GLenum& pixelFormat, GLenum& pixelType) const
    {
        // Default to RGBA8
        internalFormat = GL_RGBA8;
        pixelFormat = GL_RGBA;
        pixelType = GL_UNSIGNED_BYTE;

        // TODO: Map more formats based on TextureFormat enum.
        switch (format) {
            case TextureFormat::R8_UNORM:
                internalFormat = GL_R8;
                pixelFormat = GL_RED;
                pixelType = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::RG8_UNORM:
                internalFormat = GL_RG8;
                pixelFormat = GL_RG;
                pixelType = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::RGB8_UNORM:
                internalFormat = GL_RGB8;
                pixelFormat = GL_RGB;
                pixelType = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::RGBA8_UNORM:
                internalFormat = GL_RGBA8;
                pixelFormat = GL_RGBA;
                pixelType = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::RGBA8_SRGB:
                internalFormat = GL_SRGB8_ALPHA8;
                pixelFormat = GL_RGBA;
                pixelType = GL_UNSIGNED_BYTE;
                break;
            case TextureFormat::R16_FLOAT:
                internalFormat = GL_R16F;
                pixelFormat = GL_RED;
                pixelType = GL_FLOAT;
                break;
            case TextureFormat::RG16_FLOAT:
                internalFormat = GL_RG16F;
                pixelFormat = GL_RG;
                pixelType = GL_FLOAT;
                break;
            case TextureFormat::RGB16_FLOAT:
                internalFormat = GL_RGB16F;
                pixelFormat = GL_RGB;
                pixelType = GL_FLOAT;
                break;
            case TextureFormat::RGBA16_FLOAT:
                internalFormat = GL_RGBA16F;
                pixelFormat = GL_RGBA;
                pixelType = GL_FLOAT;
                break;
            case TextureFormat::D24_UNORM:
                internalFormat = GL_DEPTH_COMPONENT24;
                pixelFormat = GL_DEPTH_COMPONENT;
                pixelType = GL_UNSIGNED_INT;
                break;
            case TextureFormat::D32_FLOAT:
                internalFormat = GL_DEPTH_COMPONENT32F;
                pixelFormat = GL_DEPTH_COMPONENT;
                pixelType = GL_FLOAT;
                break;
            default:
                USE_LOG_WARN("Unsupported texture format, defaulting to RGBA8");
                break;
        }
    }

} // namespace USE