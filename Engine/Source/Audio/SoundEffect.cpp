// ============================================================
// Ultimate Source Engine - Sound Effect Implementation
// ============================================================

#include "stdafx.h"
#include "SoundEffect.h"
#include "AudioSystem.h"
#include "SoundBuffer.h"
#include "SoundSource.h"
#include "AudioLoader.h"
#include "Core/Engine.h"
#include "Core/Logger.h"

namespace USE {

    std::shared_ptr<SoundEffect> SoundEffect::LoadFromFile(const std::string& filename)
    {
        auto buffer = AudioLoader::LoadFromFile(filename);
        if (!buffer) {
            USE_LOG_ERROR("SoundEffect: Failed to load audio file: %s", filename.c_str());
            return nullptr;
        }
        return std::make_shared<SoundEffect>(buffer);
    }

    SoundEffect::SoundEffect(std::shared_ptr<SoundBuffer> buffer)
        : m_buffer(buffer)
    {
    }

    SoundEffect::~SoundEffect()
    {
        // m_buffer will be released automatically
    }

    std::shared_ptr<SoundSource> SoundEffect::Play(float volume, float pitch, bool loop)
    {
        auto audio = Engine::Get()->GetAudioSystem();
        if (!audio) {
            USE_LOG_ERROR("SoundEffect: AudioSystem not available");
            return nullptr;
        }

        auto source = audio->CreateSource();
        source->SetBuffer(m_buffer);
        source->SetGain(volume);
        source->SetPitch(pitch);
        source->SetLooping(loop);
        source->Play();

        return source;
    }

} // namespace USE