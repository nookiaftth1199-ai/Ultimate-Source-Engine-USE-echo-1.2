// ============================================================
// Ultimate Source Engine - Particle System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ParticleSystem.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/IRenderDevice.h"
#include "Math/MathUtils.h"
#include "Core/Logger.h"
#include <GL/gl.h> // For immediate mode (temporary; replace with proper render device calls)

namespace USE {

    // -----------------------------------------------------------------
    // ParticleEmitter
    // -----------------------------------------------------------------
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

    // -----------------------------------------------------------------
    // ParticleSystem
    // -----------------------------------------------------------------
    ParticleSystem::ParticleSystem()
        : m_device(nullptr)
        , m_defaultTexture(nullptr)
    {
    }

    ParticleSystem::~ParticleSystem()
    {
        Shutdown();
    }

    bool ParticleSystem::Initialize(IRenderDevice* device)
    {
        m_device = device;
        // Create a default white texture if needed (not implemented here)
        return true;
    }

    void ParticleSystem::Shutdown()
    {
        m_emitters.clear();
    }

    ParticleEmitter* ParticleSystem::CreateEmitter(const ParticleEmitterParams& params)
    {
        auto emitter = std::make_unique<ParticleEmitter>();
        emitter->Initialize(params);
        emitter->SetTexture(m_defaultTexture);
        ParticleEmitter* raw = emitter.get();
        m_emitters.push_back(std::move(emitter));
        return raw;
    }

    void ParticleSystem::DestroyEmitter(ParticleEmitter* emitter)
    {
        auto it = std::find_if(m_emitters.begin(), m_emitters.end(),
            [emitter](const std::unique_ptr<ParticleEmitter>& ptr) { return ptr.get() == emitter; });
        if (it != m_emitters.end()) {
            m_emitters.erase(it);
        }
    }

    void ParticleSystem::Update(float deltaTime)
    {
        for (auto& emitter : m_emitters) {
            emitter->Update(deltaTime);
        }
    }

    void ParticleSystem::Render(Camera* camera)
    {
        if (!m_device || !camera) return;

        // Get camera right and up vectors for billboarding
        Vector3 cameraRight = camera->GetRight();
        Vector3 cameraUp = camera->GetUp();

        // Save render state (simplified – use device later)
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set up orthographic projection? Actually we need to draw in world space.
        // For billboard particles, we'll draw quads facing the camera.
        // We'll use immediate mode for simplicity.

        for (auto& emitter : m_emitters) {
            Texture* tex = emitter->GetTexture() ? emitter->GetTexture() : m_defaultTexture;
            // Bind texture if available
            if (tex) {
                // For OpenGL, we'd do glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
                // We'll assume a simple bind method.
            }

            for (const auto& p : emitter->GetParticles()) {
                RenderParticle(p, cameraRight, cameraUp);
            }
        }

        glPopAttrib();
    }

    void ParticleSystem::RenderParticle(const Particle& p, const Vector3& cameraRight, const Vector3& cameraUp)
    {
        // Billboard quad
        Vector3 halfX = cameraRight * p.size * 0.5f;
        Vector3 halfY = cameraUp * p.size * 0.5f;

        Vector3 vertices[4];
        vertices[0] = p.position - halfX - halfY;
        vertices[1] = p.position + halfX - halfY;
        vertices[2] = p.position + halfX + halfY;
        vertices[3] = p.position - halfX + halfY;

        glColor4f(p.color.r, p.color.g, p.color.b, p.color.a);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
        glTexCoord2f(1, 0); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
        glTexCoord2f(1, 1); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
        glTexCoord2f(0, 1); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
        glEnd();
    }

} // namespace USE