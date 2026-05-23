// ============================================================
// Ultimate Source Engine - Particle Material Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ParticleMaterial.h"

namespace USE {

    ParticleMaterial::ParticleMaterial()
        : m_texture(nullptr)
        , m_blendMode(ParticleBlendMode::AlphaBlend)
        , m_color(1,1,1,1)
    {
    }

    ParticleMaterial::~ParticleMaterial()
    {
        // Texture is managed externally, not deleted here.
    }

} // namespace USE