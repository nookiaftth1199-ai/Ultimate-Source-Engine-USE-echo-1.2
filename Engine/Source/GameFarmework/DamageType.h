// ============================================================
// Ultimate Source Engine - Damage Type
//============================================================
//
// Defines the type of damage and associated properties.
// Used by weapons, projectiles, and health systems.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    // Basic damage categories
    enum class DamageCategory {
        Generic,
        Physical,
        Fire,
        Cold,
        Electrical,
        Poison,
        Explosive,
        Radiation,
        Healing,      // negative damage (heal)
        Custom
    };

    class DamageType {
    public:
        DamageType();
        explicit DamageType(DamageCategory category);
        virtual ~DamageType() = default;

        // Category
        void SetCategory(DamageCategory category) { m_category = category; }
        DamageCategory GetCategory() const { return m_category; }

        // Name (for custom types)
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        // Armor penetration factor (0.0 = no penetration, 1.0 = ignores armor)
        void SetArmorPenetration(float value) { m_armorPenetration = value; }
        float GetArmorPenetration() const { return m_armorPenetration; }

        // Knockback multiplier
        void SetKnockback(float multiplier) { m_knockback = multiplier; }
        float GetKnockback() const { return m_knockback; }

        // Damage over time properties (if applicable)
        void SetDamageOverTime(float damagePerSecond, float duration);
        float GetDoTDamagePerSecond() const { return m_dotDamagePerSecond; }
        float GetDoTDuration() const { return m_dotDuration; }
        bool IsDamageOverTime() const { return m_dotDamagePerSecond > 0.0f && m_dotDuration > 0.0f; }

    protected:
        DamageCategory m_category;
        std::string    m_name;
        float          m_armorPenetration;
        float          m_knockback;

        // Damage over time
        float          m_dotDamagePerSecond;
        float          m_dotDuration;
    };

} // namespace USE