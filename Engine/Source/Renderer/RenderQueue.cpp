// ============================================================
// Ultimate Source Engine - Render Queue Implementation
// ============================================================

#include "stdafx.h"
#include "RenderQueue.h"
#include "Core/Logger.h"

namespace USE {

    RenderQueue::RenderQueue()
    {
        m_commands.reserve(2048); // pre-allocate some space
    }

    void RenderQueue::AddCommand(Mesh* mesh, Material* material, const Matrix4& transform,
                                  int layer, const AABB& bounds)
    {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.material = material;
        cmd.transform = transform;
        cmd.layer = layer;
        cmd.distance = 0.0f;
        cmd.bounds = bounds;
        m_commands.push_back(cmd);
    }

    void RenderQueue::Clear()
    {
        m_commands.clear();
    }

    void RenderQueue::Sort(SortMode mode, const Vector3& cameraPosition)
    {
        if (m_commands.empty()) return;

        // If distance-based sorting is requested, compute distances first
        if (mode == SortMode::BackToFront || mode == SortMode::FrontToBack) {
            for (auto& cmd : m_commands) {
                // Use bounding box center if available, otherwise use translation
                Vector3 objPos = cmd.bounds.IsValid() ? cmd.bounds.Center()
                                                      : Vector3(cmd.transform.m[3][0],
                                                                cmd.transform.m[3][1],
                                                                cmd.transform.m[3][2]);
                cmd.distance = (objPos - cameraPosition).LengthSq(); // squared distance (faster)
            }
        }

        switch (mode) {
            case SortMode::None:
                // do nothing
                break;
            case SortMode::ByMaterial:
                std::sort(m_commands.begin(), m_commands.end(), CompareByMaterial);
                break;
            case SortMode::BackToFront:
                std::sort(m_commands.begin(), m_commands.end(), CompareBackToFront);
                break;
            case SortMode::FrontToBack:
                std::sort(m_commands.begin(), m_commands.end(), CompareFrontToBack);
                break;
            default:
                break;
        }
    }

    void RenderQueue::Cull(const Frustum& frustum)
    {
        // Remove commands that are outside the frustum
        m_commands.erase(
            std::remove_if(m_commands.begin(), m_commands.end(),
                [&frustum](const RenderCommand& cmd) {
                    // If bounding box is invalid, assume visible
                    if (!cmd.bounds.IsValid()) return false;
                    // Transform the bounding box by the object's transform
                    AABB worldBounds = cmd.bounds.Transform(cmd.transform);
                    // Test against frustum
                    return !frustum.ContainsAABB(worldBounds);
                }),
            m_commands.end()
        );
    }

    void RenderQueue::Execute(IRenderDevice* device)
    {
        if (!device) {
            USE_LOG_ERROR("RenderQueue::Execute: device is null");
            return;
        }

        Material* currentMaterial = nullptr;
        Mesh* currentMesh = nullptr;

        for (const auto& cmd : m_commands) {
            if (!cmd.mesh || !cmd.material || !cmd.mesh->IsValid() || !cmd.material->GetShader()) {
                continue; // skip invalid commands
            }

            // Bind material if changed
            if (cmd.material != currentMaterial) {
                currentMaterial = cmd.material;
                currentMaterial->Bind();
                // Set model matrix (if material expects it)
                currentMaterial->SetUniform("modelMatrix", cmd.transform);
            }

            // Bind mesh (sets vertex/index buffers)
            cmd.mesh->Bind(device);

            // Draw
            cmd.mesh->Draw(device);
        }

        // Optionally unbind last material? Not necessary.
    }

    bool RenderQueue::CompareByMaterial(const RenderCommand& a, const RenderCommand& b)
    {
        // Group by material pointer (simple)
        return a.material < b.material;
    }

    bool RenderQueue::CompareBackToFront(const RenderCommand& a, const RenderCommand& b)
    {
        return a.distance > b.distance; // farther first
    }

    bool RenderQueue::CompareFrontToBack(const RenderCommand& a, const RenderCommand& b)
    {
        return a.distance < b.distance; // closer first
    }

} // namespace USE