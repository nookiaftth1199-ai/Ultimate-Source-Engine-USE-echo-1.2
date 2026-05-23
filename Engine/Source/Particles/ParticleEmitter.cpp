// ============================================================
// Ultimate Source Engine - Particle Emitter Implementation
// ============================================================

#include "stdafx.h"
#include "ParticleEmitter.h"
#include "Math/MathUtils.h"

namespace USE {

    ParticleEmitter::ParticleEmitter()
        : m_playing(false)
        , m_paused(false)
        , m_emissionAccumulator(0.0f)
        , m_texture(nullptr)
    {
    }

    ParticleEmitter::~ParticleEmitter()
    {
    }

    void ParticleEmitter::Initialize(const ParticleEmitterParams& params)
    {
        m_params = params;
        m_particles.reserve(params.maxParticles);
        m_position = Vector3::Zero;
        m_rotation = Quaternion::Identity();
    }

    void ParticleEmitter::SetTransform(const Vector3& position, const Quaternion& rotation)
    {
        m_position = position;
        m_rotation = rotation;
    }

    void ParticleEmitter::Play()
    {
        m_playing = true;
        m_paused = false;
    }

    void ParticleEmitter::Pause()
    {
        m_paused = true;
    }

    void ParticleEmitter::Stop()
    {
        m_playing = false;
        m_paused = false;
        m_particles.clear();
        m_emissionAccumulator = 0.0f;
    }

    void ParticleEmitter::Update(float deltaTime)
    {
        if (!m_playing || m_paused) return;

        // Emit new particles
        if (m_params.emissionRate > 0.0f) {
            float emissionPerSecond = m_params.emissionRate;
            m_emissionAccumulator += deltaTime * emissionPerSecond;
            while (m_emissionAccumulator >= 1.0f && (int)m_particles.size() < m_params.maxParticles) {
                EmitParticle(1.0f / emissionPerSecond);
                m_emissionAccumulator -= 1.0f;
            }
        }

        // Update existing particles
        UpdateParticles(deltaTime);
    }

    void ParticleEmitter::EmitParticle(float deltaTime)
    {
        (void)deltaTime; // unused

        Particle p;
        // Lifetime
        p.lifetime = MathUtils::RandomFloat(m_params.lifetimeMin, m_params.lifetimeMax);
        p.initialLifetime = p.lifetime;

        // Direction: start with forward direction (0,0,1) and rotate by spread cone
        Vector3 forward(0, 0, 1);
        if (m_params.spread > 0.0f) {
            float theta = MathUtils::RandomFloat(0.0f, MathUtils::TAU);
            float phi = acosf(1.0f - MathUtils::RandomFloat(0.0f, 1.0f) * (1.0f - cosf(m_params.spread)));
            float x = sinf(phi) * cosf(theta);
            float y = sinf(phi) * sinf(theta);
            float z = cosf(phi);
            forward = Vector3(x, y, z);
        }
        forward = m_rotation * forward; // rotate by emitter orientation
        forward.Normalize();

        float speed = MathUtils::RandomFloat(m_params.speedMin, m_params.speedMax);
        p.velocity = forward * speed;

        p.position = m_position;

        // Color and size (start values)
        p.color = m_params.colorStart;
        p.size = m_params.sizeStart;

        m_particles.push_back(p);
    }

    void ParticleEmitter::UpdateParticles(float deltaTime)
    {
        for (size_t i = 0; i < m_particles.size(); ) {
            Particle& p = m_particles[i];
            p.lifetime -= deltaTime;
            if (p.lifetime <= 0.0f) {
                // Remove dead particle
                m_particles[i] = m_particles.back();
                m_particles.pop_back();
                continue;
            }

            // Update position
            p.position += p.velocity * deltaTime;

            // Interpolate color and size over lifetime
            float t = 1.0f - (p.lifetime / p.initialLifetime);
            p.color = Color::Lerp(m_params.colorStart, m_params.colorEnd, t);
            p.size = MathUtils::Lerp(m_params.sizeStart, m_params.sizeEnd, t);

            ++i;
        }
    }

} // namespace USE