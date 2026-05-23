// ============================================================
// Ultimate Source Engine - Music Stream
//============================================================
//
// Streams large audio files (music) from disk without loading
// entirely into memory. Uses OpenAL's buffer queue and a decoding
// thread to feed audio data.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <vector>

namespace USE {

    // Forward declarations
    class SoundSource;
    class AudioDecoder; // abstract decoder interface (not implemented here)

    class MusicStream {
    public:
        MusicStream();
        ~MusicStream();

        // Open an audio file for streaming. Returns true on success.
        bool OpenFromFile(const std::string& filename);

        // Close the stream and free resources.
        void Close();

        // Playback control
        void Play();
        void Pause();
        void Stop();
        bool IsPlaying() const;
        bool IsPaused() const;

        // Loop the music when it reaches the end
        void SetLooping(bool loop);
        bool IsLooping() const { return m_looping; }

        // Volume and pitch
        void SetVolume(float volume);  // 0.0 to 1.0
        float GetVolume() const;

        void SetPitch(float pitch);    // 1.0 = normal
        float GetPitch() const;

        // 3D position (if you want the music source to be positional)
        void SetPosition(const Vector3& position);
        Vector3 GetPosition() const;

        // Get total duration in seconds (if known)
        float GetDuration() const;

        // Get current playback position in seconds
        float GetPlaybackPosition() const;

        // Update – must be called regularly (e.g., every frame) to refill buffers.
        void Update();

    private:
        // Buffer queue management
        static const int NUM_BUFFERS = 4;       // number of OpenAL buffers in the queue
        static const int BUFFER_SIZE = 4096;    // size of each buffer (samples)

        std::shared_ptr<SoundSource> m_source;   // underlying sound source
        std::vector<uint32_t> m_bufferHandles;   // OpenAL buffer IDs
        std::vector<uint8_t> m_pcmBuffer;        // temporary decode buffer

        // Decoder state (abstract)
        void* m_decoder;                          // opaque decoder handle
        bool  m_streaming;                         // whether streaming is active
        std::atomic<bool> m_stopRequested;
        std::atomic<bool> m_paused;
        std::atomic<bool> m_looping;

        // Playback thread
        std::thread m_thread;
        void StreamingThreadFunc();

        // Fill a buffer with decoded data (returns number of bytes filled)
        size_t FillBuffer(uint32_t bufferId);
    };

} // namespace USE