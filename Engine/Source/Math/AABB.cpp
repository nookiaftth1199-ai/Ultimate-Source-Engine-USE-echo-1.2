// ============================================================
// Ultimate Source Engine - AABB Implementation
// ============================================================

#include "stdafx.h"
#include "AABB.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------
    AABB::AABB()
        : min(std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max())
        , max(-std::numeric_limits<float>::max(),
              -std::numeric_limits<float>::max(),
              -std::numeric_limits<float>::max())
    {}

    AABB::AABB(const Vector3& min, const Vector3& max)
        : min(min), max(max)
    {}

    AABB::AABB(const Vector3& point)
        : min(point), max(point)
    {}

    // -----------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------
    Vector3 AABB::Center() const
    {
        return (min + max) * 0.5f;
    }

    Vector3 AABB::Extents() const
    {
        return (max - min) * 0.5f;
    }

    Vector3 AABB::Size() const
    {
        return max - min;
    }

    // -----------------------------------------------------------------
    // Reset
    // -----------------------------------------------------------------
    void AABB::SetEmpty()
    {
        min.x = min.y = min.z = std::numeric_limits<float>::max();
        max.x = max.y = max.z = -std::numeric_limits<float>::max();
    }

    // -----------------------------------------------------------------
    // Validity
    // -----------------------------------------------------------------
    bool AABB::IsValid() const
    {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    // -----------------------------------------------------------------
    // Encapsulate
    // -----------------------------------------------------------------
    void AABB::Encapsulate(const Vector3& point)
    {
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.z < min.z) min.z = point.z;
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
        if (point.z > max.z) max.z = point.z;
    }

    void AABB::Encapsulate(const AABB& box)
    {
        Encapsulate(box.min);
        Encapsulate(box.max);
    }

    // -----------------------------------------------------------------
    // Contains
    // -----------------------------------------------------------------
    bool AABB::Contains(const Vector3& point) const
    {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    bool AABB::Contains(const AABB& box) const
    {
        return Contains(box.min) && Contains(box.max);
    }

    // -----------------------------------------------------------------
    // Intersection
    // -----------------------------------------------------------------
    bool AABB::Intersects(const AABB& other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    // -----------------------------------------------------------------
    // Transform by matrix (expands to contain all 8 corners)
    // -----------------------------------------------------------------
    AABB AABB::Transform(const Matrix4& matrix) const
    {
        if (!IsValid()) return *this;

        Vector3 corners[8] = {
            min,
            Vector3(max.x, min.y, min.z),
            Vector3(min.x, max.y, min.z),
            Vector3(max.x, max.y, min.z),
            Vector3(min.x, min.y, max.z),
            Vector3(max.x, min.y, max.z),
            Vector3(min.x, max.y, max.z),
            max
        };

        AABB result;
        result.SetEmpty();
        for (int i = 0; i < 8; ++i) {
            result.Encapsulate(matrix.TransformPoint(corners[i]));
        }
        return result;
    }

    // -----------------------------------------------------------------
    // Get corner by index
    // -----------------------------------------------------------------
    Vector3 AABB::GetCorner(int index) const
    {
        // 8 corners: order is (min.x, min.y, min.z) variations
        switch (index) {
            case 0: return Vector3(min.x, min.y, min.z);
            case 1: return Vector3(max.x, min.y, min.z);
            case 2: return Vector3(min.x, max.y, min.z);
            case 3: return Vector3(max.x, max.y, min.z);
            case 4: return Vector3(min.x, min.y, max.z);
            case 5: return Vector3(max.x, min.y, max.z);
            case 6: return Vector3(min.x, max.y, max.z);
            case 7: return Vector3(max.x, max.y, max.z);
            default: return Vector3::Zero;
        }
    }

    // -----------------------------------------------------------------
    // Static constants
    // -----------------------------------------------------------------
    const AABB AABB::Empty;
    const AABB AABB::Infinite(
        Vector3(-std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()),
        Vector3( std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity())
    );

} // namespace USE