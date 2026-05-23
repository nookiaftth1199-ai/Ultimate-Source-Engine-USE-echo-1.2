// ============================================================
// Ultimate Source Engine - Plane Implementation
// ============================================================

#include "stdafx.h"
#include "Plane.h"

namespace USE {

    // -----------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------
    Plane::Plane() : normal(0,1,0), distance(0) {}

    Plane::Plane(const Vector3& normal, float distance)
        : normal(normal), distance(distance)
    {}

    Plane::Plane(const Vector3& point, const Vector3& normal)
        : normal(normal), distance(normal.Dot(point))
    {}

    Plane::Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3)
    {
        Vector3 edge1 = p2 - p1;
        Vector3 edge2 = p3 - p1;
        normal = edge1.Cross(edge2).Normalized();
        distance = normal.Dot(p1);
    }

    // -----------------------------------------------------------------
    // Normalize
    // -----------------------------------------------------------------
    void Plane::Normalize()
    {
        float len = normal.Length();
        if (len > 0.0f) {
            float invLen = 1.0f / len;
            normal *= invLen;
            distance *= invLen;
        }
    }

    // -----------------------------------------------------------------
    // Get signed distance from point to plane
    // -----------------------------------------------------------------
    float Plane::GetDistance(const Vector3& point) const
    {
        return normal.Dot(point) - distance;
    }

    // -----------------------------------------------------------------
    // Project point onto plane
    // -----------------------------------------------------------------
    Vector3 Plane::Project(const Vector3& point) const
    {
        return point - normal * GetDistance(point);
    }

    // -----------------------------------------------------------------
    // Ray intersection
    // -----------------------------------------------------------------
    bool Plane::Intersects(const Ray& ray, float& t) const
    {
        float denom = ray.direction.Dot(normal);
        if (std::abs(denom) < 1e-6f)
            return false; // parallel

        t = (distance - ray.origin.Dot(normal)) / denom;
        return t >= 0.0f;
    }

    // -----------------------------------------------------------------
    // Static constants
    // -----------------------------------------------------------------
    const Plane Plane::XY(Vector3(0,0,1), 0.0f);
    const Plane Plane::XZ(Vector3(0,1,0), 0.0f);
    const Plane Plane::YZ(Vector3(1,0,0), 0.0f);

} // namespace USE