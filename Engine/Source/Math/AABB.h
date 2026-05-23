// ============================================================
// Ultimate Source Engine - Axis-Aligned Bounding Box (AABB)
// ============================================================
//
// Represents an axis-aligned bounding box defined by min and max points.
// Provides containment, intersection, and transformation utilities.
// ============================================================

#pragma once

#include "Vector3.h"
#include "Matrix4.h"
#include <limits>

namespace USE {

    class AABB {
    public:
        Vector3 min;
        Vector3 max;

        // Constructors
        inline AABB();
        inline AABB(const Vector3& min, const Vector3& max);
        inline explicit AABB(const Vector3& point); // single-point box

        // Accessors
        inline Vector3 Center() const;
        inline Vector3 Extents() const; // half-size
        inline Vector3 Size() const;

        // Reset to empty/invalid state (min = +inf, max = -inf)
        inline void SetEmpty();

        // Check if box is valid (min <= max)
        inline bool IsValid() const;

        // Encapsulate a point or another box
        inline void Encapsulate(const Vector3& point);
        inline void Encapsulate(const AABB& box);

        // Check if contains a point or another box
        inline bool Contains(const Vector3& point) const;
        inline bool Contains(const AABB& box) const;

        // Intersection tests
        inline bool Intersects(const AABB& other) const;

        // Transform by a matrix (expands to contain transformed corners)
        AABB Transform(const Matrix4& matrix) const;

        // Corner access (8 corners)
        inline Vector3 GetCorner(int index) const; // index 0..7

        // Constants
        static const AABB Empty;
        static const AABB Infinite; // min = -inf, max = +inf
    };

} // namespace USE