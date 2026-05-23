// ============================================================
// Ultimate Source Engine - Audio Loader
// ============================================================
//
// Loads sound files from disk and creates SoundBuffer objects.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class SoundBuffer;

    class AudioLoader {
    public:
        // Load a sound from a file. Returns a new SoundBuffer object
        // (owned by caller) or nullptr on failure.
        static SoundBuffer* LoadFromFile(const std::string& filename);
    };

} // namespace USE