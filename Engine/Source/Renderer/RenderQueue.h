// ============================================================
// Ultimate Source Engine - Render Queue
// ============================================================
//
// Collects draw calls and processes them in an optimized order.
// Supports sorting by material, distance, and layering.
// Also provides frustum culling and instancing support.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Frustum.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/IRenderDevice.h"
#include <vector>
#include <functional>
#include <algorithm>

namespace USE {

    // Render command structure
    struct RenderCommand {
        Mesh*       mesh;
        Material*   material;
        Matrix4     transform;
        int         layer;          // render layer (0 = opaque, 1 = transparent, etc.)
        float       distance;       // distance from camera (for sorting)
        AABB        bounds;         // bounding box for culling

        RenderCommand()
            : mesh(nullptr)
            , material(nullptr)
            , layer(0)
            , distance(0.0f)
        {}
    };

    // Sorting modes
    enum class SortMode {
        None,               // no sorting, preserve insertion order
        ByMaterial,         // group by material (for batching)
        BackToFront,        // for transparent objects
        FrontToBack         // for opaque objects (early z)
    };

    class RenderQueue {
    public:
        RenderQueue();
        ~RenderQueue() = default;

        // Add a command to the queue
        void AddCommand(Mesh* mesh, Material* material, const Matrix4& transform,
                        int layer = 0, const AABB& bounds = AABB());

        // Clear all commands
        void Clear();

        // Sort the queue according to the current mode (call before Execute)
        void Sort(SortMode mode = SortMode::ByMaterial, const Vector3& cameraPosition = Vector3::Zero);

        // Cull commands that are outside the frustum
        void Cull(const Frustum& frustum);

        // Execute all commands (render them using the provided device)
        void Execute(IRenderDevice* device);

        // Get number of commands
        size_t GetCommandCount() const { return m_commands.size(); }

    private:
        std::vector<RenderCommand> m_commands;

        // Sorting predicates
        static bool CompareByMaterial(const RenderCommand& a, const RenderCommand& b);
        static bool CompareBackToFront(const RenderCommand& a, const RenderCommand& b);
        static bool CompareFrontToBack(const RenderCommand& a, const RenderCommand& b);
    };

} // namespace USE