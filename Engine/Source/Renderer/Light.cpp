// ============================================================
// Ultimate Source Engine - Light Implementation
// ============================================================

#include "stdafx.h"
#include "Light.h"
#include "Math/MathUtils.h"

namespace USE {

    Light::Light()
        : m_type(LightType::Directional)
        , m_color(1,1,1,1)
        , m_intensity(1.0f)
        , m_position(0,0,0)
        , m_direction(0,-1,0) // default direction: down
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
        , m_innerSpotAngle(30.0f)
        , m_outerSpotAngle(45.0f)
        , m_range(100.0f)
        , m_enabled(true)
        , m_castShadows(false)
    {
    }

    void Light::SetAttenuation(float constant, float linear, float quadratic)
    {
        m_constantAttenuation = constant;
        m_linearAttenuation = linear;
        m_quadraticAttenuation = quadratic;
    }

    void Light::SetSpotAngles(float innerAngle, float outerAngle)
    {
        m_innerSpotAngle = innerAngle;
        m_outerSpotAngle = outerAngle;
    }

} // namespace USE