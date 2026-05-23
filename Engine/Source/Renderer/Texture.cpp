// ============================================================
// Ultimate Source Engine - Texture Implementation
// ============================================================

#include "stdafx.h"
#include "Texture.h"
#include "Core/Engine.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Renderer/IRenderDevice.h"

// stb_image for loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// For OpenGL backend dynamic update (temporary; should go through IRenderDevice)
#include <GL/gl.h>

namespace USE {

    Texture::Texture()
        : m_handle(0)
        , m_width(0)
        , m_height(0)
        , m_format(TextureFormat::Unknown)
        , m_device(nullptr)
    {
    }

    Texture::~Texture()
    {
        if (m_handle && m_device) {
            m_device->DestroyTexture(m_handle);
        }
    }

    bool Texture::LoadFromFile(const char* filename)
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("Texture: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("Texture: file not found: %s", filename);
            return false;
        }

        int width, height, channels;
        unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &channels, 4); // force RGBA
        if (!data) {
            USE_LOG_ERROR("Texture: failed to load image: %s", resolved.c_str());
            return false;
        }

        TextureFormat format = TextureFormat::RGBA8_UNORM;

        // Get render device from engine
        IRenderDevice* device = Engine::Get()->GetRenderer()->GetDevice();
        if (!device) {
            stbi_image_free(data);
            return false;
        }

        // Create texture via device
        uint32_t handle;
        if (!device->CreateTexture2D(width, height, format, data, handle)) {
            USE_LOG_ERROR("Texture: failed to create GPU texture for %s", filename);
            stbi_image_free(data);
            return false;
        }

        // Store metadata
        m_handle = handle;
        m_width = width;
        m_height = height;
        m_format = format;
        m_device = device;

        stbi_image_free(data);
        USE_LOG_INFO("Texture loaded: %s (%dx%d)", filename, width, height);
        return true;
    }

    bool Texture::Create(int width, int height, TextureFormat format, const void* data)
    {
        IRenderDevice* device = Engine::Get()->GetRenderer()->GetDevice();
        if (!device) return false;

        uint32_t handle;
        if (!device->CreateTexture2D(width, height, format, data, handle)) {
            USE_LOG_ERROR("Texture: failed to create texture (%dx%d)", width, height);
            return false;
        }

        if (m_handle && m_device) {
            m_device->DestroyTexture(m_handle);
        }

        m_handle = handle;
        m_width = width;
        m_height = height;
        m_format = format;
        m_device = device;

        return true;
    }

    bool Texture::Update(const void* data, int width, int height, TextureFormat format)
    {
        if (!m_handle || !m_device) return false;
        if (width != m_width || height != m_height || format != m_format) {
            USE_LOG_WARN("Texture::Update: dimensions or format mismatch");
            return false;
        }

        // Backend‑specific update – we need to call a method on IRenderDevice.
        // For now, assume OpenGL; a full implementation would have a virtual UpdateTexture.
        // We'll implement a direct OpenGL call for brevity.
        // In a real engine, IRenderDevice should have a virtual UpdateTexture2D method.
        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void Texture::Bind(int unit)
    {
        if (m_handle && m_device) {
            // In a proper abstraction, m_device->BindTexture(unit, m_handle);
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, m_handle);
        }
    }

    void Texture::Unbind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetWrap(TextureWrap s, TextureWrap t)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);

        GLenum glWrapS = GL_REPEAT, glWrapT = GL_REPEAT;
        switch (s) {
            case TextureWrap::Repeat:          glWrapS = GL_REPEAT; break;
            case TextureWrap::MirroredRepeat:  glWrapS = GL_MIRRORED_REPEAT; break;
            case TextureWrap::ClampToEdge:     glWrapS = GL_CLAMP_TO_EDGE; break;
            case TextureWrap::ClampToBorder:   glWrapS = GL_CLAMP_TO_BORDER; break;
        }
        switch (t) {
            case TextureWrap::Repeat:          glWrapT = GL_REPEAT; break;
            case TextureWrap::MirroredRepeat:  glWrapT = GL_MIRRORED_REPEAT; break;
            case TextureWrap::ClampToEdge:     glWrapT = GL_CLAMP_TO_EDGE; break;
            case TextureWrap::ClampToBorder:   glWrapT = GL_CLAMP_TO_BORDER; break;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetFilter(TextureFilter min, TextureFilter mag)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);

        GLenum glMin = GL_LINEAR, glMag = GL_LINEAR;
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
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMag);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetBorderColor(const Color& color)
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);
        float border[] = { color.r, color.g, color.b, color.a };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::GenerateMipmaps()
    {
        if (!m_handle) return;
        glBindTexture(GL_TEXTURE_2D, m_handle);
        // For OpenGL 2.0, we need gluBuild2DMipmaps or extension.
        // glGenerateMipmap requires OpenGL 3.0+.
        USE_LOG_WARN("Texture::GenerateMipmaps not implemented; use gluBuild2DMipmaps if needed.");
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture* Texture::Create()
    {
        return new Texture();
    }

} // namespace USE