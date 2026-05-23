// ============================================================
// Ultimate Source Engine - Sound Buffer Implementation
// ============================================================

#include "stdafx.h"
#include "SoundBuffer.h"
#include "Core/Logger.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {

    SoundBuffer::SoundBuffer()
        : m_bufferId(0)
        , m_frequency(0)
        , m_channels(0)
        , m_bitsPerSample(0)
        , m_dataSize(0)
    {
        alGenBuffers(1, &m_bufferId);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("SoundBuffer: Failed to generate OpenAL buffer");
            m_bufferId = 0;
        }
    }

    SoundBuffer::~SoundBuffer()
    {
        if (m_bufferId) {
            alDeleteBuffers(1, &m_bufferId);
        }
    }

    bool SoundBuffer::LoadFromMemory(const void* data, size_t size,
                                      uint32_t frequency, uint16_t channels, uint16_t bitsPerSample)
    {
        if (!m_bufferId) return false;

        // Determine OpenAL format
        ALenum format = AL_NONE;
        if (channels == 1) {
            if (bitsPerSample == 8) format = AL_FORMAT_MONO8;
            else if (bitsPerSample == 16) format = AL_FORMAT_MONO16;
        } else if (channels == 2) {
            if (bitsPerSample == 8) format = AL_FORMAT_STEREO8;
            else if (bitsPerSample == 16) format = AL_FORMAT_STEREO16;
        } else {
            USE_LOG_ERROR("SoundBuffer: Unsupported channel count: %d", channels);
            return false;
        }

        // Upload data to OpenAL buffer
        alBufferData(m_bufferId, format, data, (ALsizei)size, (ALsizei)frequency);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("SoundBuffer: Failed to upload audio data");
            return false;
        }

        m_frequency = frequency;
        m_channels = channels;
        m_bitsPerSample = bitsPerSample;
        m_dataSize = size;

        return true;
    }

} // namespace USE