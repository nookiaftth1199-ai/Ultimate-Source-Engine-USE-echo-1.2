// ============================================================
// Ultimate Source Engine - Light Component
// ============================================================
//
// Component that defines a light source in the world.
// Attach to an entity to provide lighting.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include "Math/Color.h"
#include "Math/Vector3.h"

namespace USE {

    enum class LightType {
        Directional,
        Point,
        Spot
    };

    class LightComponent : public Component {
    public:
        LightComponent();
        virtual ~LightComponent();

        // Type
        void SetType(LightType type) { m_type = type; }
        LightType GetType() const { return m_type; }

        // Basic properties
        void SetColor(const Color& color) { m_color = color; }
        const Color& GetColor() const { return m_color; }

        void SetIntensity(float intensity) { m_intensity = intensity; }
        float GetIntensity() const { return m_intensity; }

        void SetRange(float range) { m_range = range; }
        float GetRange() const { return m_range; }

        // Attenuation (for point/spot)
        void SetAttenuation(float constant, float linear, float quadratic);
        float GetConstantAttenuation() const { return m_constantAttenuation; }
        float GetLinearAttenuation() const   { return m_linearAttenuation; }
        float GetQuadraticAttenuation() const { return m_quadraticAttenuation; }

        // Spot light angles (in degrees)
        void SetSpotAngles(float innerAngle, float outerAngle);
        float GetInnerSpotAngle() const { return m_innerSpotAngle; }
        float GetOuterSpotAngle() const { return m_outerSpotAngle; }

        // Direction (for directional/spot) – derived from entity's transform
        Vector3 GetDirection() const; // world direction

        // Shadow casting
        void SetCastShadows(bool cast) { m_castShadows = cast; }
        bool GetCastShadows() const { return m_castShadows; }

        // Type name
        virtual const char* GetTypeName() const override { return "LightComponent"; }

    private:
        LightType m_type;
        Color   m_color;
        float   m_intensity;
        float   m_range;

        float m_constantAttenuation;
        float m_linearAttenuation;
        float m_quadraticAttenuation;

        float m_innerSpotAngle;
        float m_outerSpotAngle;

        bool  m_castShadows;
    };

} // namespace USE