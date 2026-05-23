// ============================================================
// Ultimate Source Engine - Frustum Culler Implementation
// ============================================================

#include "stdafx.h"
#include "FrustumCuller.h"

namespace USE {

    FrustumCuller::FrustumCuller()
    {
    }

    FrustumCuller::FrustumCuller(const Frustum& frustum)
        : m_frustum(frustum)
    {
    }

    void FrustumCuller::SetFromMatrix(const Matrix4& viewProj)
    {
        m_frustum.ExtractFromMatrix(viewProj);
    }

    bool FrustumCuller::IsVisible(const Vector3& point) const
    {
        return m_frustum.ContainsPoint(point);
    }

    bool FrustumCuller::IsVisible(const Vector3& center, float radius) const
    {
        return m_frustum.ContainsSphere(center, radius);
    }

    bool FrustumCuller::IsVisible(const AABB& aabb) const
    {
        return m_frustum.ContainsAABB(aabb);
    }

} // namespace USE