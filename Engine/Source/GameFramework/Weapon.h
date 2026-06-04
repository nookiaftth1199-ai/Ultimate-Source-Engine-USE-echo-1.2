// ============================================================
// Ultimate Source Engine - Weapon
//============================================================
//
// Base class for weapons. Can be attached to a pawn or used as a component.
// Handles firing, ammo, reloading, and different fire modes.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    // Forward declarations
    class Pawn;
    class Entity;

    // Fire modes
    enum class FireMode {
        Single,      // one shot per press
        Auto,        // continuous while pressed
        Burst        // fixed number of shots per press
    };

    class Weapon {
    public:
        Weapon();
        virtual ~Weapon();

        // Initialize weapon with basic properties
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        void SetDamage(float damage) { m_damage = damage; }
        float GetDamage() const { return m_damage; }

        void SetRange(float range) { m_range = range; }
        float GetRange() const { return m_range; }

        void SetFireRate(float roundsPerSecond); // sets delay between shots
        float GetFireRate() const { return m_fireRate; }

        void SetFireMode(FireMode mode) { m_fireMode = mode; }
        FireMode GetFireMode() const { return m_fireMode; }

        // Ammo
        void SetMaxAmmo(int maxAmmo);
        int GetMaxAmmo() const { return m_maxAmmo; }
        int GetCurrentAmmo() const { return m_currentAmmo; }
        void AddAmmo(int amount);
        bool HasAmmo() const { return m_currentAmmo > 0; }

        // Actions
        virtual void StartFiring();   // called when fire button pressed
        virtual void StopFiring();    // called when fire button released
        virtual void Fire();          // perform a single shot (if ammo available)
        virtual void Reload();

        // Update (for timing)
        virtual void Update(float deltaTime);

        // Owner (the pawn that holds this weapon)
        void SetOwner(Pawn* owner) { m_owner = owner; }
        Pawn* GetOwner() const { return m_owner; }

    protected:
        std::string m_name;
        float       m_damage;
        float       m_range;
        float       m_fireRate;          // rounds per second
        float       m_fireCooldown;       // time until next shot
        FireMode    m_fireMode;

        int         m_maxAmmo;
        int         m_currentAmmo;

        bool        m_firing;              // true when fire button is held (for auto)
        float       m_burstCount;          // for burst mode (shots left in burst)

        Pawn*       m_owner;

        // Internal shoot logic (to be overridden for projectile/trace weapons)
        virtual void PerformShot();
    };

} // namespace USE