// ============================================================
// Ultimate Source Engine - Occlusion Culler
//============================================================
//
// Performs occlusion culling using hardware queries or software
// rasterization. This is a placeholder implementation that
// currently returns false (meaning not occluded) for all tests.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/AABB.h"
#include "Math/Vector3.h"

namespace USE {

    class RenderSystem;

    class OcclusionCuller {
    public:
        OcclusionCuller();
        ~OcclusionCuller();

        // Initialize with render system.
        bool Initialize(RenderSystem* renderer);
        void Shutdown();

        // Begin/end frame – should be called before/after rendering.
        void BeginFrame();
        void EndFrame();

        // Test if a bounding volume is occluded.
        // Returns true if the volume is definitely occluded.
        bool IsOccluded(const AABB& aabb);

        // Test if a sphere is occluded.
        bool IsOccluded(const Vector3& center, float radius);

        // Enable/disable occlusion culling.
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

    private:
        RenderSystem* m_renderer;
        bool          m_enabled;
        bool          m_initialized;
    };

} // namespace USE