// ============================================================
// Ultimate Source Engine - OpenGL Vertex Array Implementation
// ============================================================

#include "stdafx.h"
#include "GLVertexArray.h"
#include "Core/Logger.h"
#include <GL/gl.h>

namespace USE {

    GLVertexArray::GLVertexArray()
        : m_indexBuffer(nullptr)
    {
    }

    GLVertexArray::~GLVertexArray()
    {
        Clear();
    }

    void GLVertexArray::AddVertexBuffer(GLBuffer* buffer, const std::vector<VertexAttribute>& attributes)
    {
        if (!buffer || !buffer->IsValid()) {
            USE_LOG_ERROR("GLVertexArray::AddVertexBuffer: invalid buffer");
            return;
        }
        if (buffer->GetType() != BufferType::Vertex) {
            USE_LOG_ERROR("GLVertexArray::AddVertexBuffer: buffer is not a vertex buffer");
            return;
        }

        VertexBufferBinding binding;
        binding.buffer = buffer;
        binding.attributes = attributes;
        m_vertexBuffers.push_back(binding);
    }

    void GLVertexArray::SetIndexBuffer(GLBuffer* buffer)
    {
        if (buffer && !buffer->IsValid()) {
            USE_LOG_ERROR("GLVertexArray::SetIndexBuffer: invalid buffer");
            return;
        }
        if (buffer && buffer->GetType() != BufferType::Index) {
            USE_LOG_ERROR("GLVertexArray::SetIndexBuffer: buffer is not an index buffer");
            return;
        }
        m_indexBuffer = buffer;
    }

    void GLVertexArray::Bind()
    {
        // Bind vertex buffers and set up attributes
        for (const auto& binding : m_vertexBuffers) {
            binding.buffer->Bind();

            for (const auto& attr : binding.attributes) {
                glEnableVertexAttribArray(attr.index);
                glVertexAttribPointer(
                    attr.index,
                    attr.size,
                    attr.type,
                    attr.normalized,
                    attr.stride,
                    reinterpret_cast<const void*>(attr.offset) // NOLINT
                );
            }
        }

        // Bind index buffer if present
        if (m_indexBuffer) {
            m_indexBuffer->Bind();
        }
    }

    void GLVertexArray::Unbind()
    {
        // Disable vertex attributes (optional, but good practice)
        for (const auto& binding : m_vertexBuffers) {
            for (const auto& attr : binding.attributes) {
                glDisableVertexAttribArray(attr.index);
            }
            binding.buffer->Unbind();
        }

        if (m_indexBuffer) {
            m_indexBuffer->Unbind();
        }
    }

    void GLVertexArray::Draw(GLenum mode, int count, int offset)
    {
        if (m_indexBuffer) {
            // Indexed draw
            glDrawElements(mode, count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(static_cast<size_t>(offset)));
        } else {
            // Non-indexed draw (use first vertex buffer's count? we assume user knows count)
            glDrawArrays(mode, 0, count);
        }
    }

    void GLVertexArray::Clear()
    {
        m_vertexBuffers.clear();
        m_indexBuffer = nullptr;
    }

} // namespace USE