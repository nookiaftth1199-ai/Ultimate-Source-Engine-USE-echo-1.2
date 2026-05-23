// ============================================================
// Ultimate Source Engine - Audio System
// ============================================================
//
// Manages audio playback using OpenAL. Provides methods to
// load sounds, play them, control listener properties, and
// manage sound sources.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <string>
#include <memory>
#include <vector>

namespace USE {

    // Forward declarations
    class SoundBuffer;
    class SoundSource;

    // Audio backend (only OpenAL for now)
    enum class AudioBackend {
        OpenAL,
        None
    };

    class AudioSystem {
    public:
        AudioSystem();
        ~AudioSystem();

        // Initialize with optional backend (default OpenAL)
        bool Initialize(AudioBackend backend = AudioBackend::OpenAL);
        void Shutdown();

        // Update (must be called once per frame to manage streaming etc.)
        void Update(float deltaTime);

        // Load a sound from file (returns a SoundBuffer handle)
        std::shared_ptr<SoundBuffer> LoadSound(const std::string& filename);

        // Create a sound source (to play a sound buffer)
        std::shared_ptr<SoundSource> CreateSource();

        // Listener properties (global)
        void SetListenerPosition(const Vector3& position);
        void SetListenerVelocity(const Vector3& velocity);
        void SetListenerOrientation(const Vector3& forward, const Vector3& up);
        void SetListenerGain(float gain);

        // Global pause/resume all sounds
        void PauseAll();
        void ResumeAll();
        void StopAll();

        // Get statistics
        int GetActiveSourceCount() const;
        int GetTotalSourceCount() const;

        // Check if system is valid
        bool IsValid() const { return m_initialized; }

    private:
        bool m_initialized;
        AudioBackend m_backend;
        void* m_device;   // OpenAL device handle (alcDevice)
        void* m_context;  // OpenAL context

        std::vector<std::shared_ptr<SoundBuffer>> m_buffers;
        std::vector<std::shared_ptr<SoundSource>> m_sources;

        // Internal: clean up finished sources
        void CleanupSources();
    };

} // namespace USE