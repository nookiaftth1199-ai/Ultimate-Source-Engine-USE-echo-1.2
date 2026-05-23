// ============================================================
// Ultimate Source Engine - Sound Source
// ============================================================
//
// Represents a sound source in 3D space. Can play a SoundBuffer,
// set position, velocity, gain, pitch, and looping properties.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"

namespace USE {

    // Forward declaration
    class SoundBuffer;

    class SoundSource {
    public:
        SoundSource();
        ~SoundSource();

        // Play/stop/pause
        void Play();
        void Stop();
        void Pause();

        // Check state
        bool IsPlaying() const;
        bool IsPaused() const;
        bool IsStopped() const;

        // Set/get the sound buffer
        void SetBuffer(std::shared_ptr<SoundBuffer> buffer);
        std::shared_ptr<SoundBuffer> GetBuffer() const { return m_buffer; }

        // Transform properties
        void SetPosition(const Vector3& pos);
        void SetVelocity(const Vector3& vel);
        void SetDirection(const Vector3& dir); // for directional sources

        // Sound properties
        void SetGain(float gain);          // 0.0 to 1.0 (default 1.0)
        void SetPitch(float pitch);        // 1.0 = normal
        void SetLooping(bool loop);
        void SetReferenceDistance(float dist); // distance at which gain is 1.0 (default 1.0)
        void SetMaxDistance(float dist);        // distance beyond which sound is inaudible (default FLT_MAX)
        void SetRolloffFactor(float factor);    // attenuation factor (default 1.0)

        // Getters
        Vector3 GetPosition() const;
        Vector3 GetVelocity() const;
        Vector3 GetDirection() const;
        float   GetGain() const;
        float   GetPitch() const;
        bool    IsLooping() const;
        float   GetReferenceDistance() const;
        float   GetMaxDistance() const;
        float   GetRolloffFactor() const;

        // Update (called by AudioSystem) – may be used for streaming or effects
        void Update(float deltaTime);

    private:
        uint32_t m_sourceId; // OpenAL source handle (ALuint)
        std::shared_ptr<SoundBuffer> m_buffer;

        // Cached properties (in case OpenAL doesn't provide getters easily)
        Vector3 m_position;
        Vector3 m_velocity;
        Vector3 m_direction;
        float   m_gain;
        float   m_pitch;
        bool    m_looping;
        float   m_referenceDistance;
        float   m_maxDistance;
        float   m_rolloffFactor;
    };

} // namespace USE