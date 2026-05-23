// ============================================================
// Ultimate Source Engine - Ray Implementation
// ============================================================

#include "stdafx.h"
#include "Ray.h"
#include "Plane.h"
#include <algorithm>
#include <cmath>

namespace USE {

    // -----------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------
    Ray::Ray() : origin(0,0,0), direction(0,0,1) {}

    Ray::Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction)
    {
        // Optionally normalize? We'll leave direction as given; user can ensure normalized.
        // Intersection methods assume direction is not zero.
    }

    // -----------------------------------------------------------------
    // GetPoint
    // -----------------------------------------------------------------
    Vector3 Ray::GetPoint(float t) const
    {
        return origin + direction * t;
    }

    // -----------------------------------------------------------------
    // DistanceToPoint
    // -----------------------------------------------------------------
    float Ray::DistanceToPoint(const Vector3& point) const
    {
        Vector3 toPoint = point - origin;
        float t = toPoint.Dot(direction); // if direction normalized, t is projection length
        Vector3 closest = origin + direction * t;
        return (point - closest).Length();
    }

    // -----------------------------------------------------------------
    // Ray-AABB intersection (slabs method)
    // -----------------------------------------------------------------
    bool Ray::Intersects(const AABB& box, float& tNear, float& tFar) const
    {
        // Compute intersection with each slab
        Vector3 invDir(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
        Vector3 t1 = (box.min - origin) * invDir;
        Vector3 t2 = (box.max - origin) * invDir;

        // Find entering and exiting t for each axis
        float tMinX = std::min(t1.x, t2.x);
        float tMaxX = std::max(t1.x, t2.x);
        float tMinY = std::min(t1.y, t2.y);
        float tMaxY = std::max(t1.y, t2.y);
        float tMinZ = std::min(t1.z, t2.z);
        float tMaxZ = std::max(t1.z, t2.z);

        // Overall intersection interval
        float tEnter = std::max(std::max(tMinX, tMinY), tMinZ);
        float tExit  = std::min(std::min(tMaxX, tMaxY), tMaxZ);

        tNear = tEnter;
        tFar = tExit;

        return tEnter <= tExit && tExit >= 0;
    }

    // -----------------------------------------------------------------
    // Ray-Plane intersection
    // -----------------------------------------------------------------
    bool Ray::Intersects(const Plane& plane, float& t) const
    {
        float denom = direction.Dot(plane.normal);
        if (std::abs(denom) < 1e-6f)
            return false; // parallel

        t = (plane.distance - origin.Dot(plane.normal)) / denom;
        return t >= 0;
    }

    // -----------------------------------------------------------------
    // Ray-Sphere intersection (returns closest positive t)
    // -----------------------------------------------------------------
    bool Ray::Intersects(const Vector3& center, float radius, float& t) const
    {
        Vector3 oc = origin - center;
        float a = direction.Dot(direction);
        float b = 2.0f * oc.Dot(direction);
        float c = oc.Dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;

        float sqrtD = sqrtf(discriminant);
        float t1 = (-b - sqrtD) / (2.0f * a);
        float t2 = (-b + sqrtD) / (2.0f * a);

        // Return the smallest positive t
        if (t1 > 0 && t2 > 0) {
            t = std::min(t1, t2);
        } else if (t1 > 0) {
            t = t1;
        } else if (t2 > 0) {
            t = t2;
        } else {
            return false; // both negative (behind origin)
        }
        return true;
    }

    // -----------------------------------------------------------------
    // Ray-Sphere intersection (return both t values)
    // -----------------------------------------------------------------
    bool Ray::Intersects(const Vector3& center, float radius, float& t1, float& t2) const
    {
        Vector3 oc = origin - center;
        float a = direction.Dot(direction);
        float b = 2.0f * oc.Dot(direction);
        float c = oc.Dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;

        float sqrtD = sqrtf(discriminant);
        t1 = (-b - sqrtD) / (2.0f * a);
        t2 = (-b + sqrtD) / (2.0f * a);
        return true;
    }

    // -----------------------------------------------------------------
    // Ray-Triangle intersection (Möller–Trumbore)
    // -----------------------------------------------------------------
    bool Ray::Intersects(const Vector3& v0, const Vector3& v1, const Vector3& v2,
                         float& t, float& u, float& v) const
    {
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        Vector3 h = direction.Cross(edge2);
        float a = edge1.Dot(h);

        if (a > -1e-6f && a < 1e-6f)
            return false; // ray parallel to triangle

        float f = 1.0f / a;
        Vector3 s = origin - v0;
        u = f * s.Dot(h);

        if (u < 0.0f || u > 1.0f)
            return false;

        Vector3 q = s.Cross(edge1);
        v = f * direction.Dot(q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        t = f * edge2.Dot(q);
        return t >= 0;
    }

} // namespace USE