// ============================================================
// Ultimate Source Engine - Particle Material
//============================================================
//
// Defines how particles are rendered: texture, blending,
// and color modulation.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"

namespace USE {

    class Texture;

    enum class ParticleBlendMode {
        Opaque,
        AlphaBlend,
        Additive,
        Multiply
    };

    class ParticleMaterial {
    public:
        ParticleMaterial();
        ~ParticleMaterial();

        // Set the texture used for particles (null = default white square)
        void SetTexture(Texture* texture) { m_texture = texture; }
        Texture* GetTexture() const { return m_texture; }

        // Blend mode
        void SetBlendMode(ParticleBlendMode mode) { m_blendMode = mode; }
        ParticleBlendMode GetBlendMode() const { return m_blendMode; }

        // Color modulation (multiplied with particle color)
        void SetColor(const Color& color) { m_color = color; }
        const Color& GetColor() const { return m_color; }

        // Whether particles are billboards (always face camera) – always true for now

    private:
        Texture* m_texture;
        ParticleBlendMode m_blendMode;
        Color m_color;
    };

} // namespace USE