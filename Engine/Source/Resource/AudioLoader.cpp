// ============================================================
// Ultimate Source Engine - Audio Loader Implementation
// ============================================================

#include "stdafx.h"
#include "AudioLoader.h"
#include "Audio/SoundBuffer.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    SoundBuffer* AudioLoader::LoadFromFile(const std::string& filename)
    {
        // Use FileSystem to resolve the path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("AudioLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("AudioLoader: Audio file not found: %s", filename.c_str());
            return nullptr;
        }

        // Create a new SoundBuffer object
        SoundBuffer* sound = new SoundBuffer();
        if (!sound->LoadFromFile(resolved)) {
            USE_LOG_ERROR("AudioLoader: Failed to load sound: %s", filename.c_str());
            delete sound;
            return nullptr;
        }

        USE_LOG_INFO("AudioLoader: Loaded sound: %s", filename.c_str());
        return sound;
    }

} // namespace USE