// ============================================================
// Ultimate Source Engine - Plane
// ============================================================
//
// Represents a plane in 3D space defined by the equation:
//   normal · point = distance
// where normal is a unit vector.
// ============================================================

#pragma once

#include "Vector3.h"
#include "Ray.h"

namespace USE {

    class Plane {
    public:
        Vector3 normal;
        float distance; // signed distance from origin along normal

        // Constructors
        inline Plane();
        inline Plane(const Vector3& normal, float distance);
        inline Plane(const Vector3& point, const Vector3& normal);
        inline Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3); // from three points

        // Normalize the plane (makes normal unit length and adjusts distance)
        inline void Normalize();

        // Get signed distance from point to plane
        inline float GetDistance(const Vector3& point) const;

        // Project point onto plane
        inline Vector3 Project(const Vector3& point) const;

        // Intersection with a ray
        inline bool Intersects(const Ray& ray, float& t) const;

        // Static constants
        static const Plane XY;   // normal = (0,0,1), distance = 0 (z=0 plane)
        static const Plane XZ;   // normal = (0,1,0), distance = 0 (y=0 plane)
        static const Plane YZ;   // normal = (1,0,0), distance = 0 (x=0 plane)
    };

} // namespace USE