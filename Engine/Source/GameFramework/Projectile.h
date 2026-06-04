// ============================================================
// Ultimate Source Engine - Projectile
//============================================================
//
// Represents a projectile fired from a weapon. Moves through the
// world, deals damage on impact, and can be affected by gravity.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Entity.h"

namespace USE {

    // Forward declarations
    class PhysicsComponent;
    class RenderComponent;
    class TransformComponent;

    class Projectile : public Entity {
    public:
        Projectile();
        virtual ~Projectile();

        // Initialize the projectile with a given velocity and damage.
        // This should be called after creation and before spawning.
        void Initialize(const Vector3& initialPosition,
                        const Vector3& velocity,
                        float damage,
                        float lifeTime = 5.0f,
                        bool gravityEnabled = true);

        // Override update
        virtual void Update(float deltaTime) override;

        // Get damage value
        float GetDamage() const { return m_damage; }

        // Set the instigator (who fired the projectile)
        void SetInstigator(Entity* instigator) { m_instigator = instigator; }
        Entity* GetInstigator() const { return m_instigator; }

    protected:
        // Called when projectile hits something.
        // Override in derived classes for custom effects.
        virtual void OnHit(Entity* hitEntity, const Vector3& hitPoint, const Vector3& hitNormal);

        TransformComponent* m_transform;
        PhysicsComponent*   m_physics;
        RenderComponent*    m_render;

        Vector3 m_initialVelocity;
        float   m_damage;
        float   m_lifeTime;
        float   m_age;
        bool    m_gravityEnabled;
        bool    m_hasHit;          // to avoid multiple hits

        Entity* m_instigator;
    };

} // namespace USE