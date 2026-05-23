// ============================================================
// Ultimate Source Engine - OpenGL Buffer Implementation
// ============================================================

#include "stdafx.h"
#include "GLBuffer.h"
#include "Core/Logger.h"

#include <GL/gl.h>

namespace USE {

    GLBuffer::GLBuffer()
        : m_handle(0)
        , m_type(BufferType::Vertex)
        , m_size(0)
    {
    }

    GLBuffer::~GLBuffer()
    {
        Destroy();
    }

    bool GLBuffer::Create(BufferType type, size_t size, const void* data, BufferUsage usage)
    {
        if (size == 0) {
            USE_LOG_ERROR("GLBuffer::Create: size cannot be zero");
            return false;
        }

        // If already created, destroy first
        if (m_handle) {
            Destroy();
        }

        glGenBuffers(1, &m_handle);
        if (!m_handle) {
            USE_LOG_ERROR("GLBuffer::Create: failed to generate buffer");
            return false;
        }

        m_type = type;
        m_size = size;

        GLenum glTarget = GetGLBufferType();
        GLenum glUsage = GetGLUsage(usage);

        glBindBuffer(glTarget, m_handle);
        glBufferData(glTarget, size, data, glUsage);
        glBindBuffer(glTarget, 0);

        // Check for OpenGL errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            USE_LOG_ERROR("GLBuffer::Create: OpenGL error 0x%04X", err);
            glDeleteBuffers(1, &m_handle);
            m_handle = 0;
            return false;
        }

        USE_LOG_DEBUG("GLBuffer created: handle=%u, type=%d, size=%zu", m_handle, (int)type, size);
        return true;
    }

    bool GLBuffer::SetData(size_t offset, size_t size, const void* data)
    {
        if (!m_handle) {
            USE_LOG_ERROR("GLBuffer::SetData: buffer not created");
            return false;
        }
        if (offset + size > m_size) {
            USE_LOG_ERROR("GLBuffer::SetData: offset+size exceeds buffer size");
            return false;
        }

        GLenum glTarget = GetGLBufferType();
        glBindBuffer(glTarget, m_handle);
        glBufferSubData(glTarget, offset, size, data);
        glBindBuffer(glTarget, 0);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            USE_LOG_ERROR("GLBuffer::SetData: OpenGL error 0x%04X", err);
            return false;
        }

        return true;
    }

    void GLBuffer::Bind()
    {
        if (m_handle) {
            glBindBuffer(GetGLBufferType(), m_handle);
        }
    }

    void GLBuffer::Unbind()
    {
        glBindBuffer(GetGLBufferType(), 0);
    }

    void GLBuffer::Destroy()
    {
        if (m_handle) {
            glDeleteBuffers(1, &m_handle);
            m_handle = 0;
            m_size = 0;
        }
    }

    GLenum GLBuffer::GetGLBufferType() const
    {
        switch (m_type) {
            case BufferType::Vertex:  return GL_ARRAY_BUFFER;
            case BufferType::Index:   return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::Uniform: return GL_UNIFORM_BUFFER; // requires GL 3.1, but we'll include anyway
            default:                   return GL_ARRAY_BUFFER;
        }
    }

    GLenum GLBuffer::GetGLUsage(BufferUsage usage) const
    {
        switch (usage) {
            case BufferUsage::Static:  return GL_STATIC_DRAW;
            case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
            case BufferUsage::Stream:  return GL_STREAM_DRAW;
            default:                    return GL_STATIC_DRAW;
        }
    }

} // namespace USE