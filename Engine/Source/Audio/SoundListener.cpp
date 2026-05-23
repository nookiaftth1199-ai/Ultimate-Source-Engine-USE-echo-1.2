// ============================================================
// Ultimate Source Engine - Sound Listener Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "SoundListener.h"
#include "Core/Logger.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {

    // Static member definitions (optional, for caching)
    Vector3 SoundListener::s_position(0,0,0);
    Vector3 SoundListener::s_velocity(0,0,0);
    Vector3 SoundListener::s_forward(0,0,-1);   // OpenAL default forward
    Vector3 SoundListener::s_up(0,1,0);          // OpenAL default up
    float   SoundListener::s_gain = 1.0f;

    void SoundListener::SetPosition(const Vector3& position)
    {
        s_position = position;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            USE_LOG_WARN("SoundListener: Error setting position (0x%04X)", err);
        }
    }

    void SoundListener::SetVelocity(const Vector3& velocity)
    {
        s_velocity = velocity;
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            USE_LOG_WARN("SoundListener: Error setting velocity (0x%04X)", err);
        }
    }

    void SoundListener::SetOrientation(const Vector3& forward, const Vector3& up)
    {
        s_forward = forward;
        s_up = up;

        // OpenAL expects an array of six floats: forward.x, forward.y, forward.z, up.x, up.y, up.z
        float orient[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, orient);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            USE_LOG_WARN("SoundListener: Error setting orientation (0x%04X)", err);
        }
    }

    void SoundListener::SetGain(float gain)
    {
        // Clamp to valid range
        if (gain < 0.0f) gain = 0.0f;
        if (gain > 1.0f) gain = 1.0f;
        s_gain = gain;
        alListenerf(AL_GAIN, gain);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            USE_LOG_WARN("SoundListener: Error setting gain (0x%04X)", err);
        }
    }

    Vector3 SoundListener::GetPosition()
    {
        return s_position;
    }

    Vector3 SoundListener::GetVelocity()
    {
        return s_velocity;
    }

    void SoundListener::GetOrientation(Vector3& forward, Vector3& up)
    {
        forward = s_forward;
        up = s_up;
    }

    float SoundListener::GetGain()
    {
        return s_gain;
    }

} // namespace USE