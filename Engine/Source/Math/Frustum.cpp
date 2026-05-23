// ============================================================
// Ultimate Source Engine - Frustum Implementation
// ============================================================

#include "stdafx.h"
#include "Frustum.h"
#include "Matrix4.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------
    Frustum::Frustum() {}

    Frustum::Frustum(const Matrix4& viewProj)
    {
        ExtractFromMatrix(viewProj);
    }

    // -----------------------------------------------------------------
    // Extract frustum planes from a view-projection matrix
    // Assumes matrix is row-major? But our Matrix4 is column-major.
    // The standard formula uses rows, so we adapt.
    // -----------------------------------------------------------------
    void Frustum::ExtractFromMatrix(const Matrix4& m)
    {
        // Left:   row3 + row0
        planes[LEFT].normal.x = m.m[3][0] + m.m[0][0];
        planes[LEFT].normal.y = m.m[3][1] + m.m[0][1];
        planes[LEFT].normal.z = m.m[3][2] + m.m[0][2];
        planes[LEFT].distance = m.m[3][3] + m.m[0][3];
        planes[LEFT].Normalize();

        // Right:  row3 - row0
        planes[RIGHT].normal.x = m.m[3][0] - m.m[0][0];
        planes[RIGHT].normal.y = m.m[3][1] - m.m[0][1];
        planes[RIGHT].normal.z = m.m[3][2] - m.m[0][2];
        planes[RIGHT].distance = m.m[3][3] - m.m[0][3];
        planes[RIGHT].Normalize();

        // Bottom: row3 + row1
        planes[BOTTOM].normal.x = m.m[3][0] + m.m[1][0];
        planes[BOTTOM].normal.y = m.m[3][1] + m.m[1][1];
        planes[BOTTOM].normal.z = m.m[3][2] + m.m[1][2];
        planes[BOTTOM].distance = m.m[3][3] + m.m[1][3];
        planes[BOTTOM].Normalize();

        // Top:    row3 - row1
        planes[TOP].normal.x = m.m[3][0] - m.m[1][0];
        planes[TOP].normal.y = m.m[3][1] - m.m[1][1];
        planes[TOP].normal.z = m.m[3][2] - m.m[1][2];
        planes[TOP].distance = m.m[3][3] - m.m[1][3];
        planes[TOP].Normalize();

        // Near:   row3 + row2
        planes[NEAR].normal.x = m.m[3][0] + m.m[2][0];
        planes[NEAR].normal.y = m.m[3][1] + m.m[2][1];
        planes[NEAR].normal.z = m.m[3][2] + m.m[2][2];
        planes[NEAR].distance = m.m[3][3] + m.m[2][3];
        planes[NEAR].Normalize();

        // Far:    row3 - row2
        planes[FAR].normal.x = m.m[3][0] - m.m[2][0];
        planes[FAR].normal.y = m.m[3][1] - m.m[2][1];
        planes[FAR].normal.z = m.m[3][2] - m.m[2][2];
        planes[FAR].distance = m.m[3][3] - m.m[2][3];
        planes[FAR].Normalize();
    }

    // -----------------------------------------------------------------
    // Contains point (inside or on plane)
    // -----------------------------------------------------------------
    bool Frustum::ContainsPoint(const Vector3& point) const
    {
        for (int i = 0; i < COUNT; ++i) {
            float dist = planes[i].GetDistance(point);
            if (dist < 0.0f)
                return false;
        }
        return true;
    }

    // -----------------------------------------------------------------
    // Contains sphere (inside or intersecting)
    // -----------------------------------------------------------------
    bool Frustum::ContainsSphere(const Vector3& center, float radius) const
    {
        for (int i = 0; i < COUNT; ++i) {
            float dist = planes[i].GetDistance(center);
            if (dist < -radius)
                return false; // sphere completely outside
        }
        return true; // inside or intersecting
    }

    // -----------------------------------------------------------------
    // Contains AABB (inside or intersecting)
    // -----------------------------------------------------------------
    bool Frustum::ContainsAABB(const AABB& box) const
    {
        for (int i = 0; i < COUNT; ++i) {
            const Plane& p = planes[i];
            // Find the vertex of the box farthest in the direction of the plane normal
            Vector3 v(
                p.normal.x > 0 ? box.max.x : box.min.x,
                p.normal.y > 0 ? box.max.y : box.min.y,
                p.normal.z > 0 ? box.max.z : box.min.z
            );
            float d = p.normal.Dot(v) - p.distance;
            if (d < 0.0f)
                return false; // box completely outside
        }
        return true; // inside or intersecting
    }

} // namespace USE