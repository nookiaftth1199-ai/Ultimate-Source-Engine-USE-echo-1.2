// ============================================================
// Ultimate Source Engine - Frustum
// ============================================================
//
// Represents a view frustum defined by six planes.
// Used for frustum culling.
// ============================================================

#pragma once

#include "Plane.h"
#include "AABB.h"
#include "Vector3.h"

namespace USE {

    class Frustum {
    public:
        enum PlaneSide {
            LEFT = 0,
            RIGHT,
            BOTTOM,
            TOP,
            NEAR,
            FAR,
            COUNT
        };

        Plane planes[COUNT];

        // Constructors
        inline Frustum();
        inline explicit Frustum(const Matrix4& viewProj); // extract from matrix

        // Extract frustum from a view-projection matrix
        void ExtractFromMatrix(const Matrix4& viewProj);

        // Test containment (returns true if inside or intersecting the frustum)
        bool ContainsPoint(const Vector3& point) const;
        bool ContainsSphere(const Vector3& center, float radius) const;
        bool ContainsAABB(const AABB& box) const;

        // Access to planes
        inline const Plane& GetPlane(PlaneSide side) const { return planes[side]; }
    };

} // namespace USE