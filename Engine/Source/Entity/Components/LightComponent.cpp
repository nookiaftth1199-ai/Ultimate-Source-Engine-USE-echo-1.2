// ============================================================
// Ultimate Source Engine - Light Component Implementation
// ============================================================

#include "stdafx.h"
#include "LightComponent.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"

namespace USE {

    LightComponent::LightComponent()
        : m_type(LightType::Directional)
        , m_color(1,1,1,1)
        , m_intensity(1.0f)
        , m_range(100.0f)
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
        , m_innerSpotAngle(30.0f)
        , m_outerSpotAngle(45.0f)
        , m_castShadows(false)
    {
    }

    LightComponent::~LightComponent()
    {
    }

    void LightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        m_constantAttenuation = constant;
        m_linearAttenuation = linear;
        m_quadraticAttenuation = quadratic;
    }

    void LightComponent::SetSpotAngles(float innerAngle, float outerAngle)
    {
        m_innerSpotAngle = innerAngle;
        m_outerSpotAngle = outerAngle;
    }

    Vector3 LightComponent::GetDirection() const
    {
        // For directional/spot, the forward direction of the entity indicates the light direction.
        Entity* owner = GetOwner();
        if (!owner) return Vector3(0, -1, 0); // default down

        TransformComponent* tc = owner->GetComponent<TransformComponent>();
        if (!tc) return Vector3(0, -1, 0);

        // Transform the local forward vector (0,0,1) to world space.
        // Assuming forward is along +Z.
        return tc->worldTransform.rotation.Rotate(Vector3(0,0,1));
    }

} // namespace USE