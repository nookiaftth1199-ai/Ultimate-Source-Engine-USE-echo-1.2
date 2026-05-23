// ============================================================
// Ultimate Source Engine - Audio Component Implementation
// ============================================================

#include "stdafx.h"
#include "AudioComponent.h"
#include "Core/Engine.h"
#include "Audio/AudioSystem.h"
#include "Audio/SoundEffect.h"
#include "Audio/SoundSource.h"
#include "Entity/Entity.h"
#include "TransformComponent.h"
#include "Core/Logger.h"

namespace USE {

    AudioComponent::AudioComponent()
        : m_volume(1.0f)
        , m_pitch(1.0f)
        , m_loop(false)
        , m_useEntityPosition(true)
        , m_loaded(false)
    {
    }

    AudioComponent::~AudioComponent()
    {
        Stop();
    }

    bool AudioComponent::LoadSound(const std::string& filename)
    {
        // Use the engine's resource manager to load the sound effect
        auto* engine = Engine::Get();
        if (!engine) return false;

        auto* audio = engine->GetAudioSystem();
        if (!audio) {
            USE_LOG_ERROR("AudioComponent: AudioSystem not available");
            return false;
        }

        // In a real engine, you'd use a resource manager to load sound effects.
        // For simplicity, we create a SoundEffect directly.
        m_effect = SoundEffect::LoadFromFile(filename);
        if (!m_effect) {
            USE_LOG_ERROR("AudioComponent: Failed to load sound: %s", filename.c_str());
            return false;
        }

        m_filename = filename;
        m_loaded = true;
        return true;
    }

    void AudioComponent::Play(bool loop)
    {
        if (!m_loaded || !m_effect) return;

        // Stop any currently playing source
        Stop();

        // Create a new source from the effect
        auto* engine = Engine::Get();
        auto* audio = engine->GetAudioSystem();
        if (!audio) return;

        m_source = m_effect->Play(m_volume, m_pitch, loop);
        if (!m_source) {
            USE_LOG_ERROR("AudioComponent: Failed to play sound");
            return;
        }

        // Set initial position if not using entity position
        if (!m_useEntityPosition) {
            m_source->SetPosition(m_customPosition);
        }
    }

    void AudioComponent::Stop()
    {
        if (m_source) {
            m_source->Stop();
            m_source.reset();
        }
    }

    void AudioComponent::Pause()
    {
        if (m_source) m_source->Pause();
    }

    void AudioComponent::Resume()
    {
        if (m_source) m_source->Play();
    }

    bool AudioComponent::IsPlaying() const
    {
        return m_source && m_source->IsPlaying();
    }

    void AudioComponent::SetVolume(float volume)
    {
        m_volume = volume;
        if (m_source) m_source->SetGain(volume);
    }

    void AudioComponent::SetPitch(float pitch)
    {
        m_pitch = pitch;
        if (m_source) m_source->SetPitch(pitch);
    }

    void AudioComponent::SetLooping(bool loop)
    {
        m_loop = loop;
        if (m_source) m_source->SetLooping(loop);
    }

    void AudioComponent::SetPosition(const Vector3& pos)
    {
        m_customPosition = pos;
        if (m_source && !m_useEntityPosition) {
            m_source->SetPosition(pos);
        }
    }

    void AudioComponent::OnAttach()
    {
        // Nothing to do on attach; sound is loaded via LoadSound later.
    }

    void AudioComponent::OnDetach()
    {
        Stop();
    }

    void AudioComponent::Update(float deltaTime)
    {
        if (m_source && m_useEntityPosition) {
            // Update source position from entity's transform
            Entity* owner = GetOwner();
            if (owner) {
                TransformComponent* tc = owner->GetComponent<TransformComponent>();
                if (tc) {
                    m_source->SetPosition(tc->worldTransform.translation);
                }
            }
        }

        // Check if source has finished playing and we should auto‑remove? Not needed.
    }

} // namespace USE