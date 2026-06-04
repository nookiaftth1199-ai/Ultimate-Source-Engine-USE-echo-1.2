// ============================================================
// Ultimate Source Engine - Projectile Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Projectile.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/PhysicsComponent.h"
#include "Entity/Components/RenderComponent.h"
#include "Physics/PhysicsWorld.h"
#include "Core/Engine.h"
#include "Core/Logger.h"

namespace USE {

    Projectile::Projectile()
        : m_transform(nullptr)
        , m_physics(nullptr)
        , m_render(nullptr)
        , m_damage(0.0f)
        , m_lifeTime(5.0f)
        , m_age(0.0f)
        , m_gravityEnabled(true)
        , m_hasHit(false)
        , m_instigator(nullptr)
    {
    }

    Projectile::~Projectile()
    {
    }

    void Projectile::Initialize(const Vector3& initialPosition,
                                const Vector3& velocity,
                                float damage,
                                float lifeTime,
                                bool gravityEnabled)
    {
        // Get or add required components
        m_transform = GetComponent<TransformComponent>();
        if (!m_transform) {
            m_transform = AddComponent<TransformComponent>();
        }
        m_transform->localTransform.translation = initialPosition;
        m_transform->UpdateWorldTransform();

        // Create physics component if needed (projectile usually uses dynamic physics)
        m_physics = GetComponent<PhysicsComponent>();
        if (!m_physics) {
            m_physics = AddComponent<PhysicsComponent>();
        }

        // Set up physics body (sphere shape)
        if (!m_physics->IsValid()) {
            PhysicsWorld* world = Engine::Get()->GetPhysicsWorld();
            if (world) {
                CollisionShape shape = CollisionShape::MakeSphere(0.2f); // small sphere
                m_physics->Initialize(world, PhysicsBodyType::Dynamic, shape, 1.0f);
            }
        }

        // Set initial velocity
        m_initialVelocity = velocity;
        m_damage = damage;
        m_lifeTime = lifeTime;
        m_gravityEnabled = gravityEnabled;

        if (m_physics && m_physics->IsValid()) {
            m_physics->SetLinearVelocity(velocity);
            // Disable gravity if needed? PhysicsComponent doesn't have that yet.
            // We'll handle via custom flag.
        }
    }

    void Projectile::Update(float deltaTime)
    {
        // Age check
        m_age += deltaTime;
        if (m_age > m_lifeTime) {
            // Destroy self
            Engine::Get()->GetWorld()->DestroyEntity(this);
            return;
        }

        if (!m_physics || !m_physics->IsValid()) return;

        // Manual gravity if enabled (since we might not want physics gravity)
        if (m_gravityEnabled) {
            Vector3 vel = m_physics->GetLinearVelocity();
            vel.y -= 9.81f * deltaTime;
            m_physics->SetLinearVelocity(vel);
        }

        // In a real implementation, you'd check for collisions via raycast or physics contact.
        // For simplicity, we'll rely on the physics component to handle collisions,
        // and we need to be notified when a collision occurs. The engine doesn't have
        // collision callbacks yet. We'll add a placeholder and maybe rely on raycasting.
        // For now, we'll do a simple raycast each frame.
        Vector3 currentPos = m_transform->worldTransform.translation;
        Vector3 vel = m_physics->GetLinearVelocity();
        Vector3 nextPos = currentPos + vel * deltaTime;

        // Raycast from current to next to detect impact
        PhysicsWorld* world = Engine::Get()->GetPhysicsWorld();
        if (world) {
            Vector3 dir = nextPos - currentPos;
            float dist = dir.Length();
            if (dist > 0.001f) {
                dir /= dist;
                RaycastResult result = world->Raycast(currentPos, dir, dist);
                if (result.hit && result.bodyId != 0) {
                    // Hit something
                    if (!m_hasHit) {
                        m_hasHit = true;
                        // Find the entity associated with the body (if any)
                        // We need a way to get entity from bodyId. Not implemented yet.
                        Entity* hitEntity = nullptr; // TODO: map bodyId -> entity
                        OnHit(hitEntity, result.point, result.normal);
                        // Destroy projectile
                        Engine::Get()->GetWorld()->DestroyEntity(this);
                        return;
                    }
                }
            }
        }

        // Update position based on physics (or manually if no physics)
        // Physics component will update entity's transform automatically if dynamic.
        // But we are not relying on physics simulation; we are manually updating.
        // Better to let physics handle it. We'll rely on physics to update transform.
        // So no need to set position here.
    }

    void Projectile::OnHit(Entity* hitEntity, const Vector3& hitPoint, const Vector3& hitNormal)
    {
        USE_LOG_INFO("Projectile hit at (%.2f,%.2f,%.2f)", hitPoint.x, hitPoint.y, hitPoint.z);
        // Apply damage to hit entity if it has a health component (not implemented)
        // For now, just log.
    }

} // namespace USE