// ============================================================
// Ultimate Source Engine - Render Command
// ============================================================
//
// Defines a command-based rendering system. Commands can be recorded
// into a command list and then executed. This allows for sorting,
// batching, and multi-threaded command generation.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Math/Rect.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace USE {

    // Forward declarations
    class IRenderDevice;

    // -----------------------------------------------------------------
    // Base class for all render commands
    // -----------------------------------------------------------------
    class RenderCommand {
    public:
        virtual ~RenderCommand() = default;

        // Execute the command on the given device
        virtual void Execute(IRenderDevice* device) = 0;

        // Get the sort key for sorting (optional, for sorting commands)
        virtual uint64_t GetSortKey() const { return 0; }

        // Get the type of command (for debugging)
        virtual const char* GetTypeName() const = 0;
    };

    // -----------------------------------------------------------------
    // Clear command (clear color/depth/stencil)
    // -----------------------------------------------------------------
    class ClearCommand : public RenderCommand {
    public:
        ClearCommand(uint32_t flags, const Color& color = Color::Black,
                     float depth = 1.0f, uint32_t stencil = 0);

        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "ClearCommand"; }

    private:
        uint32_t m_flags;
        Color    m_color;
        float    m_depth;
        uint32_t m_stencil;
    };

    // -----------------------------------------------------------------
    // Set viewport command
    // -----------------------------------------------------------------
    class SetViewportCommand : public RenderCommand {
    public:
        SetViewportCommand(const Viewport& viewport);
        SetViewportCommand(int x, int y, int width, int height,
                           float minDepth = 0.0f, float maxDepth = 1.0f);

        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetViewportCommand"; }

    private:
        Viewport m_viewport;
    };

    // -----------------------------------------------------------------
    // Set scissor rectangle command
    // -----------------------------------------------------------------
    class SetScissorCommand : public RenderCommand {
    public:
        SetScissorCommand(const Rect& rect, bool enable);
        SetScissorCommand(int x, int y, int width, int height, bool enable);

        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetScissorCommand"; }

    private:
        Rect m_rect;
        bool m_enable;
    };

    // -----------------------------------------------------------------
    // Set render target command
    // -----------------------------------------------------------------
    class SetRenderTargetCommand : public RenderCommand {
    public:
        SetRenderTargetCommand(RenderTarget* target);
        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetRenderTargetCommand"; }

    private:
        RenderTarget* m_target;
    };

    // -----------------------------------------------------------------
    // Set shader command
    // -----------------------------------------------------------------
    class SetShaderCommand : public RenderCommand {
    public:
        SetShaderCommand(Shader* shader);
        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetShaderCommand"; }

    private:
        Shader* m_shader;
    };

    // -----------------------------------------------------------------
    // Set material command
    // -----------------------------------------------------------------
    class SetMaterialCommand : public RenderCommand {
    public:
        SetMaterialCommand(Material* material);
        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetMaterialCommand"; }

    private:
        Material* m_material;
    };

    // -----------------------------------------------------------------
    // Set texture command
    // -----------------------------------------------------------------
    class SetTextureCommand : public RenderCommand {
    public:
        SetTextureCommand(uint32_t slot, Texture* texture);
        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "SetTextureCommand"; }

    private:
        uint32_t  m_slot;
        Texture*  m_texture;
    };

    // -----------------------------------------------------------------
    // Draw mesh command
    // -----------------------------------------------------------------
    class DrawMeshCommand : public RenderCommand {
    public:
        DrawMeshCommand(Mesh* mesh, Material* material, const Matrix4& transform);
        DrawMeshCommand(Mesh* mesh, Material* material, const Matrix4& transform, const AABB& bounds);

        void Execute(IRenderDevice* device) override;

        // Sort key for sorting by material and distance
        uint64_t GetSortKey() const override;

        const char* GetTypeName() const override { return "DrawMeshCommand"; }

        // For sorting
        void SetDistanceSq(float distanceSq) { m_distanceSq = distanceSq; }
        float GetDistanceSq() const { return m_distanceSq; }

    private:
        Mesh*     m_mesh;
        Material* m_material;
        Matrix4   m_transform;
        AABB      m_bounds;
        float     m_distanceSq; // squared distance for sorting
    };

    // -----------------------------------------------------------------
    // Draw indexed command (low-level)
    // -----------------------------------------------------------------
    class DrawIndexedCommand : public RenderCommand {
    public:
        DrawIndexedCommand(uint32_t indexCount, uint32_t startIndexLocation,
                           uint32_t baseVertexLocation, uint32_t instanceCount = 1);

        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "DrawIndexedCommand"; }

    private:
        uint32_t m_indexCount;
        uint32_t m_startIndexLocation;
        uint32_t m_baseVertexLocation;
        uint32_t m_instanceCount;
    };

    // -----------------------------------------------------------------
    // Draw arrays command (non-indexed)
    // -----------------------------------------------------------------
    class DrawArraysCommand : public RenderCommand {
    public:
        DrawArraysCommand(uint32_t vertexCount, uint32_t startVertexLocation,
                          uint32_t instanceCount = 1);

        void Execute(IRenderDevice* device) override;
        const char* GetTypeName() const override { return "DrawArraysCommand"; }

    private:
        uint32_t m_vertexCount;
        uint32_t m_startVertexLocation;
        uint32_t m_instanceCount;
    };

} // namespace USE