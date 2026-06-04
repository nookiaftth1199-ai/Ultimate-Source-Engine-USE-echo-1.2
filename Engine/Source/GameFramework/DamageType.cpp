// ============================================================
// Ultimate Source Engine - Damage Type Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "DamageType.h"

namespace USE {

    DamageType::DamageType()
        : m_category(DamageCategory::Generic)
        , m_armorPenetration(0.0f)
        , m_knockback(0.0f)
        , m_dotDamagePerSecond(0.0f)
        , m_dotDuration(0.0f)
    {
    }

    DamageType::DamageType(DamageCategory category)
        : m_category(category)
        , m_armorPenetration(0.0f)
        , m_knockback(0.0f)
        , m_dotDamagePerSecond(0.0f)
        , m_dotDuration(0.0f)
    {
    }

    void DamageType::SetDamageOverTime(float damagePerSecond, float duration)
    {
        m_dotDamagePerSecond = damagePerSecond;
        m_dotDuration = duration;
    }

} // namespace USE