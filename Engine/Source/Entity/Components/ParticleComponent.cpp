// ============================================================
// Ultimate Source Engine - Particle Component Implementation
// ============================================================

#include "stdafx.h"
#include "ParticleComponent.h"
#include "Core/Engine.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEffect.h"
#include "Particles/ParticleEmitter.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"
#include "Core/Logger.h"

namespace USE {

    ParticleComponent::ParticleComponent()
        : m_emitter(nullptr)
        , m_loaded(false)
    {
    }

    ParticleComponent::~ParticleComponent()
    {
        Stop();
    }

    bool ParticleComponent::LoadEffect(const std::string& filename)
    {
        auto* particleSystem = Engine::Get()->GetParticleSystem();
        if (!particleSystem) {
            USE_LOG_ERROR("ParticleComponent: ParticleSystem not available");
            return false;
        }

        // Load effect definition (this would call into resource manager)
        m_effect = particleSystem->LoadEffect(filename);
        if (!m_effect) {
            USE_LOG_ERROR("ParticleComponent: Failed to load particle effect: %s", filename.c_str());
            return false;
        }

        m_filename = filename;
        m_loaded = true;
        return true;
    }

    void ParticleComponent::Play()
    {
        if (!m_loaded || !m_effect) return;

        // Stop any existing emitter
        if (m_emitter) {
            Engine::Get()->GetParticleSystem()->DestroyEmitter(m_emitter);
            m_emitter = nullptr;
        }

        // Create a new emitter from the effect
        m_emitter = Engine::Get()->GetParticleSystem()->CreateEmitter(m_effect.get());
        if (!m_emitter) {
            USE_LOG_ERROR("ParticleComponent: Failed to create emitter");
            return;
        }

        // Set initial transform from entity
        Entity* owner = GetOwner();
        if (owner) {
            TransformComponent* tc = owner->GetComponent<TransformComponent>();
            if (tc) {
                m_emitter->SetTransform(tc->worldTransform);
            }
        }

        m_emitter->Play();
    }

    void ParticleComponent::Stop()
    {
        if (m_emitter) {
            m_emitter->Stop();
            Engine::Get()->GetParticleSystem()->DestroyEmitter(m_emitter);
            m_emitter = nullptr;
        }
    }

    void ParticleComponent::Pause()
    {
        if (m_emitter) m_emitter->Pause();
    }

    void ParticleComponent::Resume()
    {
        if (m_emitter) m_emitter->Play(); // assuming Play resumes if paused
    }

    bool ParticleComponent::IsPlaying() const
    {
        return m_emitter && m_emitter->IsPlaying();
    }

    void ParticleComponent::SetLooping(bool loop)
    {
        if (m_emitter) m_emitter->SetLooping(loop);
    }

    void ParticleComponent::SetColor(const Color& color)
    {
        if (m_emitter) m_emitter->SetColor(color);
    }

    void ParticleComponent::SetRateMultiplier(float multiplier)
    {
        if (m_emitter) m_emitter->SetRateMultiplier(multiplier);
    }

    void ParticleComponent::OnAttach()
    {
        // Nothing to do yet
    }

    void ParticleComponent::OnDetach()
    {
        Stop();
    }

    void ParticleComponent::Update(float deltaTime)
    {
        if (m_emitter) {
            // Update emitter's transform if entity moved
            Entity* owner = GetOwner();
            if (owner) {
                TransformComponent* tc = owner->GetComponent<TransformComponent>();
                if (tc) {
                    m_emitter->SetTransform(tc->worldTransform);
                }
            }
            // The emitter itself is updated by the particle system,
            // so we don't need to update it here.
        }
    }

} // namespace USE