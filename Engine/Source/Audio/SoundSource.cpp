// ============================================================
// Ultimate Source Engine - Sound Source Implementation
// ============================================================

#include "stdafx.h"
#include "SoundSource.h"
#include "SoundBuffer.h"
#include "Core/Logger.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {

    SoundSource::SoundSource()
        : m_sourceId(0)
        , m_gain(1.0f)
        , m_pitch(1.0f)
        , m_looping(false)
        , m_referenceDistance(1.0f)
        , m_maxDistance(FLT_MAX)
        , m_rolloffFactor(1.0f)
    {
        // Generate a source
        alGenSources(1, &m_sourceId);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("SoundSource: Failed to generate OpenAL source");
            m_sourceId = 0;
        }
    }

    SoundSource::~SoundSource()
    {
        Stop();
        if (m_sourceId) {
            alDeleteSources(1, &m_sourceId);
        }
    }

    void SoundSource::Play()
    {
        if (!m_sourceId || !m_buffer) return;
        alSourcePlay(m_sourceId);
    }

    void SoundSource::Stop()
    {
        if (!m_sourceId) return;
        alSourceStop(m_sourceId);
    }

    void SoundSource::Pause()
    {
        if (!m_sourceId) return;
        alSourcePause(m_sourceId);
    }

    bool SoundSource::IsPlaying() const
    {
        if (!m_sourceId) return false;
        ALint state;
        alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    bool SoundSource::IsPaused() const
    {
        if (!m_sourceId) return false;
        ALint state;
        alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }

    bool SoundSource::IsStopped() const
    {
        if (!m_sourceId) return true;
        ALint state;
        alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
        return state == AL_STOPPED || state == AL_INITIAL;
    }

    void SoundSource::SetBuffer(std::shared_ptr<SoundBuffer> buffer)
    {
        if (!m_sourceId) return;
        m_buffer = buffer;
        if (buffer) {
            alSourcei(m_sourceId, AL_BUFFER, buffer->GetHandle());
        } else {
            alSourcei(m_sourceId, AL_BUFFER, 0);
        }
    }

    void SoundSource::SetPosition(const Vector3& pos)
    {
        m_position = pos;
        if (m_sourceId) {
            alSource3f(m_sourceId, AL_POSITION, pos.x, pos.y, pos.z);
        }
    }

    void SoundSource::SetVelocity(const Vector3& vel)
    {
        m_velocity = vel;
        if (m_sourceId) {
            alSource3f(m_sourceId, AL_VELOCITY, vel.x, vel.y, vel.z);
        }
    }

    void SoundSource::SetDirection(const Vector3& dir)
    {
        m_direction = dir;
        if (m_sourceId) {
            alSource3f(m_sourceId, AL_DIRECTION, dir.x, dir.y, dir.z);
        }
    }

    void SoundSource::SetGain(float gain)
    {
        m_gain = gain;
        if (m_sourceId) {
            alSourcef(m_sourceId, AL_GAIN, gain);
        }
    }

    void SoundSource::SetPitch(float pitch)
    {
        m_pitch = pitch;
        if (m_sourceId) {
            alSourcef(m_sourceId, AL_PITCH, pitch);
        }
    }

    void SoundSource::SetLooping(bool loop)
    {
        m_looping = loop;
        if (m_sourceId) {
            alSourcei(m_sourceId, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        }
    }

    void SoundSource::SetReferenceDistance(float dist)
    {
        m_referenceDistance = dist;
        if (m_sourceId) {
            alSourcef(m_sourceId, AL_REFERENCE_DISTANCE, dist);
        }
    }

    void SoundSource::SetMaxDistance(float dist)
    {
        m_maxDistance = dist;
        if (m_sourceId) {
            alSourcef(m_sourceId, AL_MAX_DISTANCE, dist);
        }
    }

    void SoundSource::SetRolloffFactor(float factor)
    {
        m_rolloffFactor = factor;
        if (m_sourceId) {
            alSourcef(m_sourceId, AL_ROLLOFF_FACTOR, factor);
        }
    }

    Vector3 SoundSource::GetPosition() const
    {
        return m_position;
    }

    Vector3 SoundSource::GetVelocity() const
    {
        return m_velocity;
    }

    Vector3 SoundSource::GetDirection() const
    {
        return m_direction;
    }

    float SoundSource::GetGain() const
    {
        return m_gain;
    }

    float SoundSource::GetPitch() const
    {
        return m_pitch;
    }

    bool SoundSource::IsLooping() const
    {
        return m_looping;
    }

    float SoundSource::GetReferenceDistance() const
    {
        return m_referenceDistance;
    }

    float SoundSource::GetMaxDistance() const
    {
        return m_maxDistance;
    }

    float SoundSource::GetRolloffFactor() const
    {
        return m_rolloffFactor;
    }

    void SoundSource::Update(float deltaTime)
    {
        // Can be used for streaming or effects; currently no‑op.
    }

} // namespace USE