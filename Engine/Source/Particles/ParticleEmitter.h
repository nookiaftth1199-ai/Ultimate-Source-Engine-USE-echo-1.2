// ============================================================
// Ultimate Source Engine - Particle Emitter
// ============================================================
//
// Emits and manages particles over time. Each emitter has its
// own parameters (rate, lifetime, speed, size, color) and can
// be positioned in the world.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Color.h"
#include <vector>

namespace USE {

    // Forward declarations
    class Texture;

    // Single particle data
    struct Particle {
        Vector3 position;
        Vector3 velocity;
        Color   color;
        float   size;
        float   lifetime;        // remaining time (seconds)
        float   initialLifetime;
    };

    // Emitter parameters
    struct ParticleEmitterParams {
        float emissionRate;       // particles per second
        float lifetimeMin, lifetimeMax;
        float speedMin, speedMax;
        float sizeStart, sizeEnd;
        Color colorStart, colorEnd;
        Vector3 direction;        // initial direction (normalized)
        float spread;             // cone angle (radians)
        bool  loop;
        int   maxParticles;
    };

    class ParticleEmitter {
    public:
        ParticleEmitter();
        ~ParticleEmitter();

        // Initialize with parameters (call before using)
        void Initialize(const ParticleEmitterParams& params);

        // Set world transform (position/orientation)
        void SetTransform(const Vector3& position, const Quaternion& rotation = Quaternion::Identity());

        // Play/pause/stop
        void Play();
        void Pause();
        void Stop();

        bool IsPlaying() const { return m_playing; }
        bool IsPaused() const  { return m_paused; }

        // Update (must be called every frame)
        void Update(float deltaTime);

        // Get current particles (for rendering)
        const std::vector<Particle>& GetParticles() const { return m_particles; }

        // Set custom texture (if null, uses default white square)
        void SetTexture(Texture* texture) { m_texture = texture; }
        Texture* GetTexture() const { return m_texture; }

        // Parameters can be changed at runtime
        void SetEmissionRate(float rate) { m_params.emissionRate = rate; }
        void SetColor(const Color& start, const Color& end) { m_params.colorStart = start; m_params.colorEnd = end; }
        void SetSize(float start, float end) { m_params.sizeStart = start; m_params.sizeEnd = end; }

    private:
        ParticleEmitterParams m_params;
        Vector3 m_position;
        Quaternion m_rotation;
        bool m_playing;
        bool m_paused;
        float m_emissionAccumulator;

        std::vector<Particle> m_particles;
        Texture* m_texture; // optional custom texture

        void EmitParticle(float deltaTime);
        void UpdateParticles(float deltaTime);
    };

} // namespace USE