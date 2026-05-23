// ============================================================
// Ultimate Source Engine - OpenGL Vertex Array
// ============================================================
//
// Manages vertex buffer bindings and vertex attribute pointers.
// In OpenGL 2.0, this emulates a vertex array object (VAO)
// by storing the state and applying it before drawing.
// ============================================================

#pragma once

#include "stdafx.h"
#include "GLBuffer.h"
#include <vector>

namespace USE {

    // Describes a single vertex attribute
    struct VertexAttribute {
        unsigned int index;      // layout location (shader attribute index)
        int          size;       // number of components (1,2,3,4)
        GLenum       type;       // e.g., GL_FLOAT, GL_UNSIGNED_BYTE
        GLboolean    normalized; // GL_TRUE or GL_FALSE
        int          stride;     // stride in bytes between consecutive vertices
        size_t       offset;     // offset in bytes from the start of the vertex buffer
    };

    class GLVertexArray {
    public:
        GLVertexArray();
        ~GLVertexArray();

        // Add a vertex buffer with its attributes
        void AddVertexBuffer(GLBuffer* buffer, const std::vector<VertexAttribute>& attributes);

        // Set the index buffer (must be of type Index)
        void SetIndexBuffer(GLBuffer* buffer);

        // Apply the vertex array state (bind buffers and set attributes)
        void Bind();

        // Unbind (restore default state)
        void Unbind();

        // Draw using currently bound buffers
        void Draw(GLenum mode, int count, int offset = 0);

        // Clear all buffers and attributes
        void Clear();

    private:
        struct VertexBufferBinding {
            GLBuffer* buffer;
            std::vector<VertexAttribute> attributes;
        };

        std::vector<VertexBufferBinding> m_vertexBuffers;
        GLBuffer* m_indexBuffer;
    };

} // namespace USE