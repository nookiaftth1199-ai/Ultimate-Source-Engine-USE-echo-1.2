// ============================================================
// Ultimate Source Engine - Physics Component Implementation
// ============================================================

#include "stdafx.h"
#include "PhysicsComponent.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"
#include "Physics/PhysicsWorld.h"
#include "Core/Logger.h"

namespace USE {

    PhysicsComponent::PhysicsComponent()
        : m_rigidBody(nullptr)
        , m_world(nullptr)
        , m_bodyType(PhysicsBodyType::Static)
        , m_initialized(false)
    {
    }

    PhysicsComponent::~PhysicsComponent()
    {
        if (m_rigidBody) {
            m_rigidBody->Destroy();
            delete m_rigidBody;
        }
    }

    bool PhysicsComponent::Initialize(PhysicsWorld* world,
                                       PhysicsBodyType type,
                                       const CollisionShape& shape,
                                       float mass,
                                       const PhysicsMaterial& material)
    {
        if (m_initialized) return true;
        if (!world) {
            USE_LOG_ERROR("PhysicsComponent: world is null");
            return false;
        }
        m_world = world;
        m_bodyType = type;

        // Get the entity's initial transform
        Entity* owner = GetOwner();
        if (!owner) {
            USE_LOG_ERROR("PhysicsComponent: no owner entity");
            return false;
        }

        TransformComponent* tc = owner->GetComponent<TransformComponent>();
        if (!tc) {
            USE_LOG_ERROR("PhysicsComponent: entity missing TransformComponent");
            return false;
        }

        // Create the rigid body using the shape and initial transform
        m_rigidBody = new RigidBody();

        // We need to convert our body type to RigidBodyType
        RigidBodyType rigidType;
        switch (type) {
            case PhysicsBodyType::Static:    rigidType = RigidBodyType::Static; break;
            case PhysicsBodyType::Dynamic:   rigidType = RigidBodyType::Dynamic; break;
            case PhysicsBodyType::Kinematic: rigidType = RigidBodyType::Kinematic; break;
            default: rigidType = RigidBodyType::Static; break;
        }

        // Initialize the rigid body with the appropriate shape helper
        // For now, we only support box, sphere, capsule via separate methods.
        // We'll add a switch based on shape type.
        bool success = false;
        switch (shape.type) {
            case CollisionShapeType::Box:
                success = m_rigidBody->Initialize(world, rigidType,
                                                   shape.box.halfExtents, mass);
                break;
            case CollisionShapeType::Sphere:
                success = m_rigidBody->Initialize(world, rigidType,
                                                   shape.sphere.radius, mass);
                break;
            case CollisionShapeType::Capsule:
                success = m_rigidBody->Initialize(world, rigidType,
                                                   shape.capsule.radius,
                                                   shape.capsule.height, mass);
                break;
            default:
                USE_LOG_ERROR("PhysicsComponent: unsupported shape type for initialization");
                break;
        }

        if (!success) {
            USE_LOG_ERROR("PhysicsComponent: failed to create rigid body");
            delete m_rigidBody;
            m_rigidBody = nullptr;
            return false;
        }

        // Set the transform from the entity
        m_rigidBody->SetTransform(tc->worldTransform.ToMatrix());

        m_initialized = true;
        return true;
    }

    void PhysicsComponent::Update(float deltaTime)
    {
        if (!m_initialized || !m_rigidBody) return;

        if (m_bodyType == PhysicsBodyType::Kinematic) {
            // Kinematic: we control the body from the entity's transform
            Entity* owner = GetOwner();
            if (owner) {
                TransformComponent* tc = owner->GetComponent<TransformComponent>();
                if (tc) {
                    m_rigidBody->SetTransform(tc->worldTransform.ToMatrix());
                }
            }
        }
        // For static/dynamic, nothing to do here (physics world updates them)
    }

    void PhysicsComponent::LateUpdate(float deltaTime)
    {
        if (!m_initialized || !m_rigidBody) return;

        if (m_bodyType == PhysicsBodyType::Dynamic) {
            // Update entity transform from physics body
            Entity* owner = GetOwner();
            if (owner) {
                TransformComponent* tc = owner->GetComponent<TransformComponent>();
                if (tc) {
                    Matrix4 bodyMat = m_rigidBody->GetTransform();
                    // We need to extract translation and rotation.
                    // The TransformComponent expects a Transform (pos, rot, scale).
                    // For simplicity, we just update the local transform.
                    // Note: scale is not handled by physics.
                    Vector3 pos = Vector3(bodyMat.m[3][0], bodyMat.m[3][1], bodyMat.m[3][2]);
                    // Extract rotation as quaternion (not trivial). We'll just update translation.
                    // In a real engine, you'd have a more complete transform extraction.
                    // For now, we only update position.
                    tc->localTransform.translation = pos;
                    tc->UpdateWorldTransform(); // propagate
                }
            }
        }
    }

    void PhysicsComponent::ApplyForce(const Vector3& force, const Vector3* relPos)
    {
        if (m_initialized && m_rigidBody) {
            m_rigidBody->ApplyForce(force, relPos);
        }
    }

    void PhysicsComponent::ApplyImpulse(const Vector3& impulse, const Vector3* relPos)
    {
        if (m_initialized && m_rigidBody) {
            m_rigidBody->ApplyImpulse(impulse, relPos);
        }
    }

} // namespace USE