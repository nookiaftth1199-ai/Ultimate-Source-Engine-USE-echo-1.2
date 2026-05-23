// ============================================================
// Ultimate Source Engine - Collision Shape
// ============================================================
//
// Describes the collision geometry of a physics object.
// Can be a sphere, box, capsule, cylinder, convex hull, or triangle mesh.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <vector>

namespace USE {

    // Supported shape types
    enum class CollisionShapeType {
        Sphere,
        Box,
        Capsule,
        Cylinder,
        ConvexHull,
        TriangleMesh,
        Compound
    };

    // Collision shape data structure
    struct CollisionShape {
        CollisionShapeType type;

        union {
            struct { float radius; } sphere;
            struct { Vector3 halfExtents; } box;
            struct { float radius; float height; } capsule;
            struct { float radius; float height; } cylinder;
        };

        // For convex hull and triangle mesh, these point to user-managed data.
        // The user must ensure the data remains valid for the lifetime of the shape.
        const std::vector<Vector3>* vertices;
        const std::vector<uint32_t>* indices;
        bool convex; // for mesh: true if convex hull, false if triangle mesh

        // Optional user data (e.g., pointer to engine-specific mesh)
        void* userData;

        CollisionShape();
        ~CollisionShape() = default;

        // Factory methods for common shapes
        static CollisionShape MakeSphere(float radius);
        static CollisionShape MakeBox(const Vector3& halfExtents);
        static CollisionShape MakeCapsule(float radius, float height);
        static CollisionShape MakeCylinder(float radius, float height);
        static CollisionShape MakeConvexHull(const std::vector<Vector3>& vertices);
        static CollisionShape MakeTriangleMesh(const std::vector<Vector3>& vertices,
                                                const std::vector<uint32_t>& indices);
    };

} // namespace USE