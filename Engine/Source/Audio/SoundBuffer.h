// ============================================================
// Ultimate Source Engine - Sound Buffer
// ============================================================
//
// Represents an audio buffer containing raw PCM data.
// Can be loaded from a file (via AudioLoader) and attached
// to SoundSource objects for playback.
// ============================================================

#pragma once

#include "stdafx.h"
#include <cstdint>
#include <vector>

namespace USE {

    class SoundBuffer {
    public:
        SoundBuffer();
        ~SoundBuffer();

        // Load from raw PCM data (used by AudioLoader)
        bool LoadFromMemory(const void* data, size_t size,
                            uint32_t frequency, uint16_t channels, uint16_t bitsPerSample);

        // Get OpenAL buffer handle
        uint32_t GetHandle() const { return m_bufferId; }

        // Get audio properties
        uint32_t GetFrequency() const { return m_frequency; }
        uint16_t GetChannels() const { return m_channels; }
        uint16_t GetBitsPerSample() const { return m_bitsPerSample; }
        size_t GetDataSize() const { return m_dataSize; }

    private:
        uint32_t m_bufferId;          // OpenAL buffer handle
        uint32_t m_frequency;
        uint16_t m_channels;
        uint16_t m_bitsPerSample;
        size_t   m_dataSize;

        // Optional: keep a copy of data for potential reuse (not needed for OpenAL)
        // std::vector<uint8_t> m_data;
    };

} // namespace USE