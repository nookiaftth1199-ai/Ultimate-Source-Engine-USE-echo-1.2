// ============================================================
// Ultimate Source Engine - Weapon Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Weapon.h"
#include "GameFramework/Pawn.h"
#include "Core/Logger.h"

namespace USE {

    Weapon::Weapon()
        : m_damage(10.0f)
        , m_range(100.0f)
        , m_fireRate(2.0f)          // 2 shots per second
        , m_fireCooldown(0.0f)
        , m_fireMode(FireMode::Single)
        , m_maxAmmo(30)
        , m_currentAmmo(30)
        , m_firing(false)
        , m_burstCount(0)
        , m_owner(nullptr)
    {
    }

    Weapon::~Weapon()
    {
    }

    void Weapon::SetFireRate(float roundsPerSecond)
    {
        m_fireRate = roundsPerSecond;
        // m_fireCooldown will be computed as 1.0f / m_fireRate when needed
    }

    void Weapon::SetMaxAmmo(int maxAmmo)
    {
        m_maxAmmo = maxAmmo;
        if (m_currentAmmo > m_maxAmmo) m_currentAmmo = m_maxAmmo;
    }

    void Weapon::AddAmmo(int amount)
    {
        m_currentAmmo += amount;
        if (m_currentAmmo > m_maxAmmo) m_currentAmmo = m_maxAmmo;
        if (m_currentAmmo < 0) m_currentAmmo = 0;
    }

    void Weapon::StartFiring()
    {
        if (!HasAmmo()) {
            // maybe dry fire sound
            return;
        }

        m_firing = true;

        switch (m_fireMode) {
            case FireMode::Single:
                Fire();
                m_firing = false; // single shot, don't continue
                break;

            case FireMode::Auto:
                // Fire immediately, then continue based on cooldown
                Fire();
                break;

            case FireMode::Burst:
                // Assume burst of 3 shots
                m_burstCount = 3;
                Fire();
                break;
        }
    }

    void Weapon::StopFiring()
    {
        m_firing = false;
        m_burstCount = 0;
    }

    void Weapon::Fire()
    {
        if (!HasAmmo()) return;

        // Check cooldown
        if (m_fireCooldown > 0.0f) return;

        // Perform the shot
        PerformShot();

        // Reduce ammo
        m_currentAmmo--;
        USE_LOG_INFO("Weapon fired. Ammo left: %d", m_currentAmmo);

        // Set cooldown
        m_fireCooldown = 1.0f / m_fireRate;

        // Burst handling
        if (m_fireMode == FireMode::Burst) {
            m_burstCount--;
            if (m_burstCount <= 0) {
                m_firing = false;
            }
        }
    }

    void Weapon::PerformShot()
    {
        // Default implementation: raycast from owner's position forward
        if (!m_owner) return;

        // Simple hit scan
        Vector3 start = m_owner->GetPosition() + m_owner->GetForward() * 1.0f; // from "muzzle"
        Vector3 end = start + m_owner->GetForward() * m_range;

        // Use physics world to raycast
        // (We need access to PhysicsWorld; we could get it from Engine)
        // For now, just log
        USE_LOG_INFO("Weapon: Raycast from (%.2f,%.2f,%.2f) to (%.2f,%.2f,%.2f)",
                     start.x, start.y, start.z, end.x, end.y, end.z);
    }

    void Weapon::Reload()
    {
        // Simple reload: set current ammo to max
        m_currentAmmo = m_maxAmmo;
        USE_LOG_INFO("Weapon reloaded. Ammo: %d", m_currentAmmo);
    }

    void Weapon::Update(float deltaTime)
    {
        // Update cooldown
        if (m_fireCooldown > 0.0f) {
            m_fireCooldown -= deltaTime;
        }

        // Handle automatic firing
        if (m_firing) {
            if (m_fireMode == FireMode::Auto) {
                Fire();
            } else if (m_fireMode == FireMode::Burst && m_burstCount > 0) {
                Fire();
            }
        }
    }

} // namespace USE