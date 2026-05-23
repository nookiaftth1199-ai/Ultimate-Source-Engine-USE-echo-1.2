// ============================================================
// Ultimate Source Engine - Collision Shape Implementation
// ============================================================

#include "stdafx.h"
#include "CollisionShape.h"

namespace USE {

    CollisionShape::CollisionShape()
        : type(CollisionShapeType::Box)
        , vertices(nullptr)
        , indices(nullptr)
        , convex(false)
        , userData(nullptr)
    {
        box.halfExtents = Vector3(1,1,1);
    }

    CollisionShape CollisionShape::MakeSphere(float radius)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Sphere;
        shape.sphere.radius = radius;
        return shape;
    }

    CollisionShape CollisionShape::MakeBox(const Vector3& halfExtents)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Box;
        shape.box.halfExtents = halfExtents;
        return shape;
    }

    CollisionShape CollisionShape::MakeCapsule(float radius, float height)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Capsule;
        shape.capsule.radius = radius;
        shape.capsule.height = height;
        return shape;
    }

    CollisionShape CollisionShape::MakeCylinder(float radius, float height)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Cylinder;
        shape.cylinder.radius = radius;
        shape.cylinder.height = height;
        return shape;
    }

    CollisionShape CollisionShape::MakeConvexHull(const std::vector<Vector3>& vertices)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::ConvexHull;
        shape.vertices = &vertices; // careful: pointer to local? Not safe if passed.
        // Better to store copy? We'll just set pointer; caller must keep alive.
        shape.convex = true;
        return shape;
    }

    CollisionShape CollisionShape::MakeTriangleMesh(const std::vector<Vector3>& vertices,
                                                    const std::vector<uint32_t>& indices)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::TriangleMesh;
        shape.vertices = &vertices;
        shape.indices = &indices;
        shape.convex = false;
        return shape;
    }

} // namespace USE