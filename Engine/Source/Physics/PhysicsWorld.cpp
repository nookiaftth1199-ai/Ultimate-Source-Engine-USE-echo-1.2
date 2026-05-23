// ============================================================
// Ultimate Source Engine - Physics World Implementation
// ============================================================

#include "stdafx.h"
#include "PhysicsWorld.h"
#include "Core/Logger.h"

namespace USE {

    PhysicsWorld::PhysicsWorld()
        : m_impl(nullptr)
    {
    }

    PhysicsWorld::~PhysicsWorld()
    {
        Shutdown();
    }

    bool PhysicsWorld::Initialize(PhysicsBackend backend, const PhysicsWorldSettings& settings)
    {
        // We cannot instantiate an IPhysicsSystem directly because it's abstract.
        // We need to use the PhysicsSystem dispatcher or create backend directly.
        // For simplicity, we'll assume we have a global physics system factory,
        // but here we'll create a new backend via the PhysicsSystem class (which is a dispatcher).
        // However, PhysicsSystem is also an IPhysicsSystem, so we can use it.
        // To avoid including PhysicsSystem.h, we'll forward declare a factory function.
        // For now, we'll use a simple approach: ifdef each backend.
        // In a real engine, you'd have a factory.

        if (m_impl) {
            Shutdown();
        }

        // Create the appropriate backend (this duplicates logic from PhysicsSystem, but okay)
        switch (backend) {
#ifdef USE_BULLET
            case PhysicsBackend::Bullet:
                m_impl = new BulletPhysicsSystem();
                break;
#endif
#ifdef USE_PHYSX
            case PhysicsBackend::PhysX:
                m_impl = new PhysXPhysicsSystem();
                break;
#endif
#ifdef USE_JOLT
            case PhysicsBackend::Jolt:
                m_impl = new JoltPhysicsSystem();
                break;
#endif
            default:
                USE_LOG_ERROR("PhysicsWorld: Requested backend not available.");
                return false;
        }

        if (!m_impl->Initialize(settings)) {
            USE_LOG_ERROR("PhysicsWorld: Failed to initialize backend.");
            delete m_impl;
            m_impl = nullptr;
            return false;
        }

        USE_LOG_INFO("PhysicsWorld initialized with backend '%s'.", GetBackendName());
        return true;
    }

    void PhysicsWorld::Shutdown()
    {
        if (m_impl) {
            // Destroy all bodies (the backend will handle it, but we also clear mapping)
            m_bodyToEntity.clear();
            m_impl->Shutdown();
            delete m_impl;
            m_impl = nullptr;
        }
    }

    void PhysicsWorld::Update(float deltaTime)
    {
        if (m_impl) {
            m_impl->Update(deltaTime);
            // In a real implementation, we would collect collision events here
            // and invoke the callback. That requires backend-specific contact
            // processing. We'll omit it for brevity.
        }
    }

    // -----------------------------------------------------------------
    // Body creation helpers
    // -----------------------------------------------------------------
    uint32_t PhysicsWorld::CreateBoxBody(const Vector3& halfExtents, const Matrix4& transform,
                                         float mass, uint32_t entityId)
    {
        CollisionShape shape = MakeBoxShape(halfExtents);
        RigidBodyDesc desc;
        desc.mass = mass;
        desc.initialTransform = transform;
        uint32_t bodyId = m_impl->CreateRigidBody(desc, shape);
        if (bodyId != 0 && entityId != 0) {
            m_bodyToEntity[bodyId] = entityId;
        }
        return bodyId;
    }

    uint32_t PhysicsWorld::CreateSphereBody(float radius, const Matrix4& transform,
                                            float mass, uint32_t entityId)
    {
        CollisionShape shape = MakeSphereShape(radius);
        RigidBodyDesc desc;
        desc.mass = mass;
        desc.initialTransform = transform;
        uint32_t bodyId = m_impl->CreateRigidBody(desc, shape);
        if (bodyId != 0 && entityId != 0) {
            m_bodyToEntity[bodyId] = entityId;
        }
        return bodyId;
    }

    uint32_t PhysicsWorld::CreateCapsuleBody(float radius, float height, const Matrix4& transform,
                                             float mass, uint32_t entityId)
    {
        CollisionShape shape = MakeCapsuleShape(radius, height);
        RigidBodyDesc desc;
        desc.mass = mass;
        desc.initialTransform = transform;
        uint32_t bodyId = m_impl->CreateRigidBody(desc, shape);
        if (bodyId != 0 && entityId != 0) {
            m_bodyToEntity[bodyId] = entityId;
        }
        return bodyId;
    }

    uint32_t PhysicsWorld::CreateMeshBody(const std::vector<Vector3>& vertices,
                                          const std::vector<uint32_t>& indices,
                                          const Matrix4& transform, bool convex,
                                          float mass, uint32_t entityId)
    {
        CollisionShape shape = MakeMeshShape(vertices, indices, convex);
        RigidBodyDesc desc;
        desc.mass = mass;
        desc.initialTransform = transform;
        uint32_t bodyId = m_impl->CreateRigidBody(desc, shape);
        if (bodyId != 0 && entityId != 0) {
            m_bodyToEntity[bodyId] = entityId;
        }
        return bodyId;
    }

    void PhysicsWorld::DestroyBody(uint32_t bodyId)
    {
        m_impl->DestroyRigidBody(bodyId);
        m_bodyToEntity.erase(bodyId);
    }

    // -----------------------------------------------------------------
    // Body state queries
    // -----------------------------------------------------------------
    Matrix4 PhysicsWorld::GetBodyTransform(uint32_t bodyId) const
    {
        return m_impl->GetBodyTransform(bodyId);
    }

    void PhysicsWorld::SetBodyTransform(uint32_t bodyId, const Matrix4& transform)
    {
        m_impl->SetBodyTransform(bodyId, transform);
    }

    Vector3 PhysicsWorld::GetBodyLinearVelocity(uint32_t bodyId) const
    {
        // Not directly in IPhysicsSystem; we need to add it.
        // For now, we'll assume we have a method. If not, we'll comment out.
        // return m_impl->GetLinearVelocity(bodyId);
        USE_LOG_WARN("PhysicsWorld::GetBodyLinearVelocity not implemented");
        return Vector3::Zero;
    }

    void PhysicsWorld::SetBodyLinearVelocity(uint32_t bodyId, const Vector3& velocity)
    {
        m_impl->SetLinearVelocity(bodyId, velocity);
    }

    Vector3 PhysicsWorld::GetBodyAngularVelocity(uint32_t bodyId) const
    {
        USE_LOG_WARN("PhysicsWorld::GetBodyAngularVelocity not implemented");
        return Vector3::Zero;
    }

    void PhysicsWorld::SetBodyAngularVelocity(uint32_t bodyId, const Vector3& velocity)
    {
        m_impl->SetAngularVelocity(bodyId, velocity);
    }

    void PhysicsWorld::ApplyForce(uint32_t bodyId, const Vector3& force, const Vector3* relPos)
    {
        m_impl->ApplyForce(bodyId, force, relPos);
    }

    void PhysicsWorld::ApplyImpulse(uint32_t bodyId, const Vector3& impulse, const Vector3* relPos)
    {
        m_impl->ApplyImpulse(bodyId, impulse, relPos);
    }

    RaycastResult PhysicsWorld::Raycast(const Vector3& origin, const Vector3& direction,
                                        float maxDistance, uint32_t layerMask)
    {
        return m_impl->Raycast(origin, direction, maxDistance, layerMask);
    }

    void PhysicsWorld::DebugDraw()
    {
        m_impl->DebugDraw();
    }

    PhysicsBackend PhysicsWorld::GetBackend() const
    {
        // Not in IPhysicsSystem; we'd need to store it.
        return PhysicsBackend::None;
    }

    const char* PhysicsWorld::GetBackendName() const
    {
        return m_impl ? m_impl->GetBackendName() : "None";
    }

    // -----------------------------------------------------------------
    // Shape helpers
    // -----------------------------------------------------------------
    CollisionShape PhysicsWorld::MakeBoxShape(const Vector3& halfExtents)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Box;
        shape.box.halfExtents = halfExtents;
        return shape;
    }

    CollisionShape PhysicsWorld::MakeSphereShape(float radius)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Sphere;
        shape.sphere.radius = radius;
        return shape;
    }

    CollisionShape PhysicsWorld::MakeCapsuleShape(float radius, float height)
    {
        CollisionShape shape;
        shape.type = CollisionShapeType::Capsule;
        shape.capsule.radius = radius;
        shape.capsule.height = height;
        return shape;
    }

    CollisionShape PhysicsWorld::MakeMeshShape(const std::vector<Vector3>& vertices,
                                               const std::vector<uint32_t>& indices,
                                               bool convex)
    {
        CollisionShape shape;
        shape.type = convex ? CollisionShapeType::ConvexHull : CollisionShapeType::TriangleMesh;
        // We would need to store the vertex/index data somewhere.
        // For simplicity, we'll set userData to point to the data, but the data must persist.
        // This is just a sketch; in a real engine you'd have a more robust mesh shape.
        struct MeshData {
            std::vector<Vector3> verts;
            std::vector<uint32_t> idxs;
        };
        MeshData* data = new MeshData;
        data->verts = vertices;
        data->idxs = indices;
        shape.userData = data;
        return shape;
    }

} // namespace USE