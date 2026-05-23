// ============================================================
// Ultimate Source Engine - Particle System
//============================================================
//
// Manages particle emitters and updates/renders particles.
// Supports sprite‑based particles with color, size, and lifetime.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include <vector>
#include <memory>

namespace USE {

    // Forward declarations
    class IRenderDevice;
    class Camera;
    class Texture;

    // Single particle data
    struct Particle {
        Vector3 position;
        Vector3 velocity;
        Color   color;
        float   size;
        float   lifetime;   // remaining time (seconds)
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

    // -----------------------------------------------------------------
    // Main Particle System (manager)
    // -----------------------------------------------------------------
    class ParticleSystem {
    public:
        ParticleSystem();
        ~ParticleSystem();

        // Initialize with render device (for drawing)
        bool Initialize(IRenderDevice* device);
        void Shutdown();

        // Create a new emitter (takes ownership)
        ParticleEmitter* CreateEmitter(const ParticleEmitterParams& params);
        void DestroyEmitter(ParticleEmitter* emitter);

        // Update all emitters
        void Update(float deltaTime);

        // Render all emitters using current camera
        void Render(Camera* camera);

        // Set default particle texture (used when emitter has none)
        void SetDefaultTexture(Texture* texture) { m_defaultTexture = texture; }

    private:
        IRenderDevice* m_device;
        Texture* m_defaultTexture;
        std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;

        // Simple shader for billboard rendering (placeholder)
        void RenderParticle(const Particle& p, const Vector3& cameraRight, const Vector3& cameraUp);
    };

} // namespace USE