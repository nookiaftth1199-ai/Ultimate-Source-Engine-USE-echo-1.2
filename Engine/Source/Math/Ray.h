// ============================================================
// Ultimate Source Engine - Ray
// ============================================================
//
// Represents a 3D ray defined by an origin and a direction.
// Provides intersection tests with various primitives.
// ============================================================

#pragma once

#include "Vector3.h"
#include "AABB.h"

namespace USE {

    class Ray {
    public:
        Vector3 origin;
        Vector3 direction;

        // Constructors
        inline Ray();
        inline Ray(const Vector3& origin, const Vector3& direction);

        // Get point along ray at parameter t
        inline Vector3 GetPoint(float t) const;

        // Distance from ray origin to a point
        inline float DistanceToPoint(const Vector3& point) const;

        // Intersection tests

        // Ray-AABB: returns true if hit, and outputs t at entry and exit (can be same for just hit)
        inline bool Intersects(const AABB& box, float& tNear, float& tFar) const;

        // Ray-Plane: returns true if hit, outputs t
        inline bool Intersects(const class Plane& plane, float& t) const;

        // Ray-Sphere: returns true if hit, outputs t (closest) and optionally both intersections
        inline bool Intersects(const Vector3& center, float radius, float& t) const;
        inline bool Intersects(const Vector3& center, float radius, float& t1, float& t2) const;

        // Ray-Triangle (Möller–Trumbore): returns true if hit, outputs t and barycentric coordinates
        inline bool Intersects(const Vector3& v0, const Vector3& v1, const Vector3& v2,
                               float& t, float& u, float& v) const;
    };

} // namespace USE