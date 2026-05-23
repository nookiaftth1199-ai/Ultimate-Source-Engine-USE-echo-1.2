// ============================================================
// Ultimate Source Engine - Sound Effect
// ============================================================
//
// Represents a playable sound effect. Loads a sound file and
// provides a method to create a SoundSource for playback.
// Multiple simultaneous plays are supported by creating multiple
// sources.
// ============================================================

#pragma once

#include "stdafx.h"
#include <memory>
#include <string>

namespace USE {

    // Forward declarations
    class SoundBuffer;
    class SoundSource;
    class AudioSystem;

    class SoundEffect {
    public:
        // Load a sound effect from file. Returns nullptr on failure.
        static std::shared_ptr<SoundEffect> LoadFromFile(const std::string& filename);

        // Constructor (private, use LoadFromFile)
        ~SoundEffect();

        // Play the sound effect. Creates a new SoundSource, sets its
        // buffer and parameters, and returns the source. The caller
        // can further control playback via the returned source.
        std::shared_ptr<SoundSource> Play(float volume = 1.0f,
                                          float pitch = 1.0f,
                                          bool loop = false);

        // Get the underlying sound buffer (shared)
        std::shared_ptr<SoundBuffer> GetBuffer() const { return m_buffer; }

    private:
        explicit SoundEffect(std::shared_ptr<SoundBuffer> buffer);

        std::shared_ptr<SoundBuffer> m_buffer;
    };

} // namespace USE