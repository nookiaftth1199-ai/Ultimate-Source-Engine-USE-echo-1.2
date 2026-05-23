// ============================================================
// Ultimate Source Engine - Light
// ============================================================
//
// Represents a light source in the scene. Supports directional,
// point, and spot lights. Contains common light properties such
// as color, intensity, attenuation, and direction/position.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace USE {

    // Light types
    enum class LightType {
        Directional,
        Point,
        Spot
    };

    class Light {
    public:
        Light();
        ~Light() = default;

        // Type
        void SetType(LightType type) { m_type = type; }
        LightType GetType() const { return m_type; }

        // Basic properties
        void SetColor(const Color& color) { m_color = color; }
        const Color& GetColor() const { return m_color; }

        void SetIntensity(float intensity) { m_intensity = intensity; }
        float GetIntensity() const { return m_intensity; }

        // Position (for point/spot)
        void SetPosition(const Vector3& pos) { m_position = pos; }
        const Vector3& GetPosition() const { return m_position; }

        // Direction (for directional/spot)
        void SetDirection(const Vector3& dir) { m_direction = dir.Normalized(); }
        Vector3 GetDirection() const { return m_direction; }

        // Attenuation (for point/spot)
        void SetAttenuation(float constant, float linear, float quadratic);
        float GetConstantAttenuation() const { return m_constantAttenuation; }
        float GetLinearAttenuation() const   { return m_linearAttenuation; }
        float GetQuadraticAttenuation() const { return m_quadraticAttenuation; }

        // Spot light parameters (cone angles in degrees)
        void SetSpotAngles(float innerAngle, float outerAngle);
        float GetInnerSpotAngle() const { return m_innerSpotAngle; }
        float GetOuterSpotAngle() const { return m_outerSpotAngle; }

        // Range (maximum distance light affects)
        void SetRange(float range) { m_range = range; }
        float GetRange() const { return m_range; }

        // Enable/disable
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        // Shadow casting
        void SetCastShadows(bool cast) { m_castShadows = cast; }
        bool GetCastShadows() const { return m_castShadows; }

    private:
        LightType m_type;

        Color   m_color;
        float   m_intensity;

        Vector3 m_position;     // for point/spot
        Vector3 m_direction;    // for directional/spot (normalized)

        // Attenuation coefficients (for point/spot)
        float m_constantAttenuation;
        float m_linearAttenuation;
        float m_quadraticAttenuation;

        // Spot light cone angles (in degrees)
        float m_innerSpotAngle;
        float m_outerSpotAngle;

        float m_range;          // maximum distance
        bool  m_enabled;
        bool  m_castShadows;
    };

} // namespace USE