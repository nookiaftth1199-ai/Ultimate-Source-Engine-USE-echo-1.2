// ============================================================
// Ultimate Source Engine - Physics Debug Drawing
//============================================================
//
// Provides functions to visualize physics objects (rigid bodies,
// collision shapes, joints) using the engine's DebugDraw system.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Physics/IPhysicsSystem.h"
#include "Debug/DebugDraw.h"

namespace USE {

    class PhysicsWorld;
    class RigidBody;

    class PhysicsDebug {
    public:
        // Enable/disable global physics debug drawing
        static void SetEnabled(bool enabled) { s_enabled = enabled; }
        static bool IsEnabled() { return s_enabled; }

        // Draw an entire physics world
        static void DrawWorld(PhysicsWorld* world);

        // Draw a single rigid body (if you have its ID)
        static void DrawRigidBody(uint32_t bodyId, IPhysicsSystem* physics);

        // Draw a collision shape at a given transform
        static void DrawShape(const CollisionShape& shape, const Matrix4& transform, const Color& color = Color::White);

    private:
        static bool s_enabled;
    };

} // namespace USE