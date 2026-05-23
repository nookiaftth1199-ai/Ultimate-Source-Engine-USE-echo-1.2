// ============================================================
// Ultimate Source Engine - OpenGL Buffer
// ============================================================
//
// Wrapper for OpenGL buffer objects (VBO, IBO, etc.).
// Supports creation, data upload, binding, and destruction.
// ============================================================

#pragma once

#include "stdafx.h"

namespace USE {

    // Buffer types
    enum class BufferType {
        Vertex,   // GL_ARRAY_BUFFER
        Index,    // GL_ELEMENT_ARRAY_BUFFER
        Uniform,  // GL_UNIFORM_BUFFER (requires OpenGL 3.1+)
        // Other types can be added as needed
    };

    // Usage hints (for glBufferData)
    enum class BufferUsage {
        Static,   // GL_STATIC_DRAW
        Dynamic,  // GL_DYNAMIC_DRAW
        Stream    // GL_STREAM_DRAW
    };

    class GLBuffer {
    public:
        GLBuffer();
        ~GLBuffer();

        // Create a buffer of given type and size, optionally with initial data
        bool Create(BufferType type, size_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::Static);

        // Upload new data (full or partial)
        bool SetData(size_t offset, size_t size, const void* data);

        // Bind/unbind the buffer
        void Bind();
        void Unbind();

        // Get OpenGL handle
        unsigned int GetHandle() const { return m_handle; }

        // Get buffer properties
        BufferType GetType() const { return m_type; }
        size_t GetSize() const { return m_size; }
        bool IsValid() const { return m_handle != 0; }

        // Destroy the buffer (also called in destructor)
        void Destroy();

    private:
        unsigned int  m_handle;
        BufferType    m_type;
        size_t        m_size;

        // Convert engine enums to OpenGL enums
        GLenum GetGLBufferType() const;
        GLenum GetGLUsage(BufferUsage usage) const;
    };

} // namespace USE