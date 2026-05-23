// ============================================================
// Ultimate Source Engine - Physics Debug Drawing Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PhysicsDebug.h"
#include "PhysicsWorld.h"
#include "Core/Engine.h"

namespace USE {

    bool PhysicsDebug::s_enabled = true;

    void PhysicsDebug::DrawWorld(PhysicsWorld* world)
    {
        if (!s_enabled || !world) return;

        // This requires the physics backend to provide a way to iterate over bodies.
        // Since IPhysicsSystem doesn't have an enumeration method, we'll rely on the
        // backend's own debug drawing (which many physics engines provide).
        // We'll call the backend's DebugDraw() method, which is supposed to render
        // its own debug visuals. This is simpler and more efficient.
        world->DebugDraw();
    }

    void PhysicsDebug::DrawRigidBody(uint32_t bodyId, IPhysicsSystem* physics)
    {
        if (!s_enabled || !physics) return;

        // Get body transform
        Matrix4 transform = physics->GetBodyTransform(bodyId);

        // We need the shape associated with the body. Since IPhysicsSystem doesn't provide
        // a way to retrieve the shape, we cannot draw it generically.
        // Instead, we rely on the backend's DebugDraw() method to draw all bodies.
        // This function is not implemented.
    }

    void PhysicsDebug::DrawShape(const CollisionShape& shape, const Matrix4& transform, const Color& color)
    {
        if (!s_enabled) return;

        auto* debug = Engine::Get()->GetDebugDraw();
        if (!debug) return;

        // Draw based on shape type
        switch (shape.type) {
            case CollisionShapeType::Sphere: {
                // Draw sphere using DebugDraw (approximate with rings)
                Vector3 center = transform.TransformPoint(Vector3::Zero);
                debug->DrawSphere(center, shape.sphere.radius, color, 16);
                break;
            }
            case CollisionShapeType::Box: {
                Vector3 center = transform.TransformPoint(Vector3::Zero);
                Vector3 half = shape.box.halfExtents;
                debug->DrawBox(center, half, color);
                break;
            }
            case CollisionShapeType::Capsule: {
                // Approximate: draw a cylinder with hemispherical ends
                // This is complex; for now, just draw a cylinder.
                Vector3 center = transform.TransformPoint(Vector3::Zero);
                debug->DrawCylinder(center, shape.capsule.radius, shape.capsule.height, color, 16);
                // Could also draw two spheres at ends and lines.
                break;
            }
            case CollisionShapeType::Cylinder: {
                Vector3 center = transform.TransformPoint(Vector3::Zero);
                debug->DrawCylinder(center, shape.cylinder.radius, shape.cylinder.height, color, 16);
                break;
            }
            case CollisionShapeType::ConvexHull:
            case CollisionShapeType::TriangleMesh:
            default:
                // Not implemented
                break;
        }
    }

} // namespace USE