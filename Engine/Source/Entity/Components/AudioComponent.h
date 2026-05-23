// ============================================================
// Ultimate Source Engine - Audio Component
// ============================================================
//
// Component that attaches a sound effect or stream to an entity.
// Can be used for ambient sounds, footsteps, weapon sounds, etc.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include <string>
#include <memory>

namespace USE {

    // Forward declarations
    class SoundSource;
    class SoundEffect;

    class AudioComponent : public Component {
    public:
        AudioComponent();
        virtual ~AudioComponent();

        // Load a sound file (effect) to be played. This creates a SoundEffect.
        // If the file is a long music stream, you might use a different method.
        bool LoadSound(const std::string& filename);

        // Play the sound. If looping is true, the sound will loop.
        void Play(bool loop = false);

        // Stop the sound.
        void Stop();

        // Pause/resume.
        void Pause();
        void Resume();

        // Check if playing.
        bool IsPlaying() const;

        // Set volume (0.0 to 1.0).
        void SetVolume(float volume);

        // Set pitch (1.0 = normal).
        void SetPitch(float pitch);

        // Set whether the sound should loop.
        void SetLooping(bool loop);

        // Set 3D position (if not set, the entity's transform will be used).
        void SetPosition(const Vector3& pos);
        void UseEntityPosition(bool use) { m_useEntityPosition = use; }

        // Overrides from Component
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update(float deltaTime) override;

        virtual const char* GetTypeName() const override { return "AudioComponent"; }

    private:
        std::shared_ptr<SoundSource> m_source;    // The active sound source (if playing)
        std::shared_ptr<SoundEffect> m_effect;    // The loaded sound effect
        std::string                  m_filename;  // Original filename
        float                        m_volume;
        float                        m_pitch;
        bool                         m_loop;
        Vector3                      m_customPosition;
        bool                         m_useEntityPosition;
        bool                         m_loaded;
    };

} // namespace USE