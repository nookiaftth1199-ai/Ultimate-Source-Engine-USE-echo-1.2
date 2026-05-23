// ============================================================
// Ultimate Source Engine - Frustum Culler
// ============================================================
//
// Helper class for frustum culling of bounding volumes.
// Wraps a Frustum and provides convenient methods to test
// points, spheres, and AABBs against it.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Frustum.h"
#include "Math/AABB.h"
#include "Math/Vector3.h"

namespace USE {

    class FrustumCuller {
    public:
        FrustumCuller();
        explicit FrustumCuller(const Frustum& frustum);
        ~FrustumCuller() = default;

        // Update the frustum from a view‑projection matrix.
        void SetFromMatrix(const Matrix4& viewProj);

        // Directly set the internal frustum.
        void SetFrustum(const Frustum& frustum) { m_frustum = frustum; }
        const Frustum& GetFrustum() const { return m_frustum; }

        // Test containment.
        bool IsVisible(const Vector3& point) const;
        bool IsVisible(const Vector3& center, float radius) const; // sphere
        bool IsVisible(const AABB& aabb) const;

    private:
        Frustum m_frustum;
    };

} // namespace USE