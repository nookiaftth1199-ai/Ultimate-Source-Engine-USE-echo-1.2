// ============================================================
// Ultimate Source Engine - Audio System Implementation
// ============================================================

#include "stdafx.h"
#include "AudioSystem.h"
#include "Audio/SoundBuffer.h"
#include "Audio/SoundSource.h"
#include "Audio/AudioLoader.h"
#include "Core/Logger.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {

    AudioSystem::AudioSystem()
        : m_initialized(false)
        , m_backend(AudioBackend::None)
        , m_device(nullptr)
        , m_context(nullptr)
    {
    }

    AudioSystem::~AudioSystem()
    {
        Shutdown();
    }

    bool AudioSystem::Initialize(AudioBackend backend)
    {
        if (m_initialized) return true;

        if (backend != AudioBackend::OpenAL) {
            USE_LOG_ERROR("AudioSystem: Only OpenAL backend is supported");
            return false;
        }

        // Open default audio device
        ALCdevice* device = alcOpenDevice(nullptr);
        if (!device) {
            USE_LOG_ERROR("AudioSystem: Failed to open OpenAL device");
            return false;
        }

        // Create context
        ALCcontext* context = alcCreateContext(device, nullptr);
        if (!context) {
            alcCloseDevice(device);
            USE_LOG_ERROR("AudioSystem: Failed to create OpenAL context");
            return false;
        }

        alcMakeContextCurrent(context);

        // Check for errors
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioSystem: OpenAL error during initialization: 0x%04X", error);
            alcDestroyContext(context);
            alcCloseDevice(device);
            return false;
        }

        m_device = device;
        m_context = context;
        m_backend = backend;
        m_initialized = true;

        USE_LOG_INFO("AudioSystem initialized with OpenAL");
        return true;
    }

    void AudioSystem::Shutdown()
    {
        if (!m_initialized) return;

        // Stop all sources
        StopAll();

        // Clear buffers and sources (shared_ptr will delete)
        m_buffers.clear();
        m_sources.clear();

        // Destroy OpenAL context and device
        ALCcontext* context = (ALCcontext*)m_context;
        ALCdevice* device = (ALCdevice*)m_device;

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        m_initialized = false;
        USE_LOG_INFO("AudioSystem shut down");
    }

    void AudioSystem::Update(float deltaTime)
    {
        // Clean up stopped sources
        CleanupSources();

        // Update 3D positions of active sources (if any)
        // (The sources themselves handle this via their own Update)
    }

    std::shared_ptr<SoundBuffer> AudioSystem::LoadSound(const std::string& filename)
    {
        auto buffer = AudioLoader::LoadFromFile(filename);
        if (buffer) {
            m_buffers.push_back(buffer);
        }
        return buffer;
    }

    std::shared_ptr<SoundSource> AudioSystem::CreateSource()
    {
        auto source = std::make_shared<SoundSource>();
        m_sources.push_back(source);
        return source;
    }

    void AudioSystem::SetListenerPosition(const Vector3& position)
    {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    void AudioSystem::SetListenerVelocity(const Vector3& velocity)
    {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void AudioSystem::SetListenerOrientation(const Vector3& forward, const Vector3& up)
    {
        float orient[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, orient);
    }

    void AudioSystem::SetListenerGain(float gain)
    {
        alListenerf(AL_GAIN, gain);
    }

    void AudioSystem::PauseAll()
    {
        for (auto& src : m_sources) {
            src->Pause();
        }
    }

    void AudioSystem::ResumeAll()
    {
        for (auto& src : m_sources) {
            src->Play();
        }
    }

    void AudioSystem::StopAll()
    {
        for (auto& src : m_sources) {
            src->Stop();
        }
    }

    int AudioSystem::GetActiveSourceCount() const
    {
        int count = 0;
        for (auto& src : m_sources) {
            if (src->IsPlaying()) count++;
        }
        return count;
    }

    int AudioSystem::GetTotalSourceCount() const
    {
        return (int)m_sources.size();
    }

    void AudioSystem::CleanupSources()
    {
        // Remove sources that are finished (not playing, paused, etc.)
        // Usually sources are reused, not destroyed, so we might not need to remove.
        // But if sources are created and never cleaned up, we might want to remove stopped ones.
        // For simplicity, we keep them; they are cheap.
        // Alternatively, we could have the SoundSource auto-destroy when stopped? Not implemented.
    }

} // namespace USE