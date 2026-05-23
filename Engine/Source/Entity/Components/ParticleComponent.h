// ============================================================
// Ultimate Source Engine - Particle Component
// ============================================================
//
// Component that attaches a particle effect to an entity.
// The effect can be loaded from a file and played, looped, etc.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include <string>
#include <memory>

namespace USE {

    // Forward declarations
    class ParticleEmitter;
    class ParticleEffect;

    class ParticleComponent : public Component {
    public:
        ParticleComponent();
        virtual ~ParticleComponent();

        // Load a particle effect from a file (e.g., .particle or .json).
        // Returns true on success.
        bool LoadEffect(const std::string& filename);

        // Play the particle effect (start emitting).
        void Play();

        // Stop emitting (immediately destroy all particles).
        void Stop();

        // Pause/unpause the emitter (freezes simulation).
        void Pause();
        void Resume();

        // Check if the emitter is playing.
        bool IsPlaying() const;

        // Set the effect to loop (only applicable if effect is looping).
        void SetLooping(bool loop);

        // Set the color multiplier.
        void SetColor(const Color& color);

        // Set the emission rate multiplier.
        void SetRateMultiplier(float multiplier);

        // Overrides from Component
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update(float deltaTime) override;

        virtual const char* GetTypeName() const override { return "ParticleComponent"; }

    private:
        std::shared_ptr<ParticleEffect> m_effect;   // loaded effect definition
        ParticleEmitter*                m_emitter;  // active emitter instance (if any)
        std::string                     m_filename;
        bool                            m_loaded;
    };

} // namespace USE