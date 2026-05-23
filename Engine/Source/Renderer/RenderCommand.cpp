// ============================================================
// Ultimate Source Engine - Render Command Implementation
// ============================================================

#include "stdafx.h"
#include "RenderCommand.h"
#include "Renderer/IRenderDevice.h"
#include "Core/Logger.h"

namespace USE {

    // -----------------------------------------------------------------
    // ClearCommand
    // -----------------------------------------------------------------
    ClearCommand::ClearCommand(uint32_t flags, const Color& color,
                               float depth, uint32_t stencil)
        : m_flags(flags)
        , m_color(color)
        , m_depth(depth)
        , m_stencil(stencil)
    {}

    void ClearCommand::Execute(IRenderDevice* device)
    {
        if (device) {
            device->Clear(m_flags, m_color, m_depth, m_stencil);
        }
    }

    // -----------------------------------------------------------------
    // SetViewportCommand
    // -----------------------------------------------------------------
    SetViewportCommand::SetViewportCommand(const Viewport& viewport)
        : m_viewport(viewport)
    {}

    SetViewportCommand::SetViewportCommand(int x, int y, int width, int height,
                                           float minDepth, float maxDepth)
        : m_viewport((float)x, (float)y, (float)width, (float)height, minDepth, maxDepth)
    {}

    void SetViewportCommand::Execute(IRenderDevice* device)
    {
        if (device) {
            device->SetViewport((int)m_viewport.x, (int)m_viewport.y,
                                (int)m_viewport.width, (int)m_viewport.height);
        }
    }

    // -----------------------------------------------------------------
    // SetScissorCommand
    // -----------------------------------------------------------------
    SetScissorCommand::SetScissorCommand(const Rect& rect, bool enable)
        : m_rect(rect), m_enable(enable)
    {}

    SetScissorCommand::SetScissorCommand(int x, int y, int width, int height, bool enable)
        : m_rect(x, y, width, height), m_enable(enable)
    {}

    void SetScissorCommand::Execute(IRenderDevice* device)
    {
        if (device) {
            device->EnableScissor(m_enable);
            if (m_enable) {
                device->SetScissorRect(m_rect.x, m_rect.y, m_rect.width, m_rect.height);
            }
        }
    }

    // -----------------------------------------------------------------
    // SetRenderTargetCommand
    // -----------------------------------------------------------------
    SetRenderTargetCommand::SetRenderTargetCommand(RenderTarget* target)
        : m_target(target)
    {}

    void SetRenderTargetCommand::Execute(IRenderDevice* device)
    {
        if (device) {
            if (m_target) {
                m_target->Bind();
            } else {
                // 0 means backbuffer
                device->SetRenderTarget(0);
            }
        }
    }

    // -----------------------------------------------------------------
    // SetShaderCommand
    // -----------------------------------------------------------------
    SetShaderCommand::SetShaderCommand(Shader* shader)
        : m_shader(shader)
    {}

    void SetShaderCommand::Execute(IRenderDevice* device)
    {
        if (m_shader) {
            m_shader->Bind();
        }
    }

    // -----------------------------------------------------------------
    // SetMaterialCommand
    // -----------------------------------------------------------------
    SetMaterialCommand::SetMaterialCommand(Material* material)
        : m_material(material)
    {}

    void SetMaterialCommand::Execute(IRenderDevice* device)
    {
        if (m_material) {
            m_material->Bind();
        }
    }

    // -----------------------------------------------------------------
    // SetTextureCommand
    // -----------------------------------------------------------------
    SetTextureCommand::SetTextureCommand(uint32_t slot, Texture* texture)
        : m_slot(slot), m_texture(texture)
    {}

    void SetTextureCommand::Execute(IRenderDevice* device)
    {
        if (m_texture) {
            m_texture->Bind(m_slot);
        } else {
            // unbind? Not implemented
        }
    }

    // -----------------------------------------------------------------
    // DrawMeshCommand
    // -----------------------------------------------------------------
    DrawMeshCommand::DrawMeshCommand(Mesh* mesh, Material* material, const Matrix4& transform)
        : m_mesh(mesh)
        , m_material(material)
        , m_transform(transform)
        , m_distanceSq(0.0f)
    {}

    DrawMeshCommand::DrawMeshCommand(Mesh* mesh, Material* material, const Matrix4& transform, const AABB& bounds)
        : m_mesh(mesh)
        , m_material(material)
        , m_transform(transform)
        , m_bounds(bounds)
        , m_distanceSq(0.0f)
    {}

    void DrawMeshCommand::Execute(IRenderDevice* device)
    {
        if (!m_mesh || !m_material || !device) return;

        // Bind material (sets shader, uniforms, textures)
        m_material->SetUniform("modelMatrix", m_transform);
        m_material->Bind();

        // Bind mesh buffers
        m_mesh->Bind(device);

        // Draw
        m_mesh->Draw(device);
    }

    uint64_t DrawMeshCommand::GetSortKey() const
    {
        // Simple sort key: material pointer (lower bits) + distance quantized into higher bits
        // For demo: combine distance (converted to 32-bit fixed point) with material pointer
        uint64_t materialKey = reinterpret_cast<uint64_t>(m_material) & 0xFFFFF; // lower 20 bits
        uint64_t distanceKey = static_cast<uint64_t>(m_distanceSq * 1000.0f) & 0xFFFFFFFF; // 32 bits
        return (distanceKey << 20) | materialKey;
    }

    // -----------------------------------------------------------------
    // DrawIndexedCommand
    // -----------------------------------------------------------------
    DrawIndexedCommand::DrawIndexedCommand(uint32_t indexCount, uint32_t startIndexLocation,
                                           uint32_t baseVertexLocation, uint32_t instanceCount)
        : m_indexCount(indexCount)
        , m_startIndexLocation(startIndexLocation)
        , m_baseVertexLocation(baseVertexLocation)
        , m_instanceCount(instanceCount)
    {}

    void DrawIndexedCommand::Execute(IRenderDevice* device)
    {
        if (device) {
            // If instanceCount > 1, we need DrawIndexedInstanced; for simplicity, we ignore for now.
            for (uint32_t i = 0; i < m_instanceCount; ++i) {
                device->DrawIndexed(m_indexCount, m_startIndexLocation, m_baseVertexLocation);
            }
        }
    }

    // -----------------------------------------------------------------
    // DrawArraysCommand
    // -----------------------------------------------------------------
    DrawArraysCommand::DrawArraysCommand(uint32_t vertexCount, uint32_t startVertexLocation,
                                         uint32_t instanceCount)
        : m_vertexCount(vertexCount)
        , m_startVertexLocation(startVertexLocation)
        , m_instanceCount(instanceCount)
    {}

    void DrawArraysCommand::Execute(IRenderDevice* device)
    {
        // Not implemented in IRenderDevice yet; would need a DrawArrays method.
        USE_LOG_WARN("DrawArraysCommand not implemented in IRenderDevice");
    }

} // namespace USE