// ============================================================
// Ultimate Source Engine - Particle Effect
//============================================================
//
// Defines a named particle effect that can be loaded from a
// JSON file and used to create ParticleEmitter instances.
// ============================================================

#pragma once

#include "stdafx.h"
#include "ParticleEmitter.h"
#include <string>
#include <memory>

namespace USE {

    class ParticleEffect {
    public:
        ParticleEffect();
        ~ParticleEffect() = default;

        // Load effect definition from a JSON file.
        // Returns true on success.
        bool LoadFromFile(const std::string& filename);

        // Create a new emitter instance from this effect definition.
        // The caller owns the returned pointer.
        ParticleEmitter* CreateEmitter() const;

        // Getters for effect parameters (read-only)
        const std::string& GetName() const { return m_name; }
        const ParticleEmitterParams& GetParams() const { return m_params; }

    private:
        std::string m_name;
        ParticleEmitterParams m_params;
    };

} // namespace USE