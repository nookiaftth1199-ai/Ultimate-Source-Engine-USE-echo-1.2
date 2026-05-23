// ============================================================
// Ultimate Source Engine - Audio Filter
//============================================================
//
// Base class and concrete implementations for audio filters
// using OpenAL EFX (Effects Extension). Filters can be attached
// to sound sources to modify their output (e.g., reverb, echo,
// low‑pass, etc.).
// ============================================================

#pragma once

#include "stdafx.h"
#include <cstdint>

namespace USE {

    // Forward declarations
    class SoundSource;

    // -----------------------------------------------------------------
    // Base class for all audio filters
    // -----------------------------------------------------------------
    class AudioFilter {
    public:
        AudioFilter();
        virtual ~AudioFilter();

        // Initialize the filter. Returns true if OpenAL EFX is available.
        virtual bool Initialize() = 0;

        // Attach this filter to a sound source. Some filters may use
        // auxiliary effect slots instead of direct filter attachments.
        virtual void AttachToSource(SoundSource* source) = 0;

        // Detach from the source.
        virtual void DetachFromSource(SoundSource* source) = 0;

        // Enable/disable the filter.
        virtual void SetEnabled(bool enabled);
        bool IsEnabled() const { return m_enabled; }

    protected:
        uint32_t m_filterId;   // OpenAL filter handle (ALuint)
        uint32_t m_effectId;   // OpenAL effect handle (for effects)
        uint32_t m_slotId;     // OpenAL auxiliary effect slot (for effects)
        bool     m_enabled;
    };

    // -----------------------------------------------------------------
    // Reverb filter (uses AL_EFFECT_REVERB)
    // -----------------------------------------------------------------
    class AudioReverbFilter : public AudioFilter {
    public:
        AudioReverbFilter();
        ~AudioReverbFilter();

        bool Initialize() override;
        void AttachToSource(SoundSource* source) override;
        void DetachFromSource(SoundSource* source) override;

        // Parameter setters (see OpenAL EFX spec for details)
        void SetDensity(float density);         // 0.0 – 1.0 (default 1.0)
        void SetDiffusion(float diffusion);     // 0.0 – 1.0 (default 1.0)
        void SetGain(float gain);               // 0.0 – 1.0 (default 0.32)
        void SetGainHF(float gainHF);           // 0.0 – 1.0 (default 0.89)
        void SetDecayTime(float time);          // seconds (default 1.49)
        void SetDecayHFRatio(float ratio);      // 0.1 – 2.0 (default 0.83)
        void SetReflectionsGain(float gain);    // 0.0 – 3.16 (default 0.05)
        void SetReflectionsDelay(float delay);  // seconds (default 0.007)
        void setLateReverbGain(float gain);     // 0.0 – 10.0 (default 1.26)
        void setLateReverbDelay(float delay);   // seconds (default 0.011)
        void SetAirAbsorptionGainHF(float gain);// 0.0 – 1.0 (default 0.994)
        void SetRoomRolloffFactor(float factor);// 0.0 – 10.0 (default 0.0)

    private:
        float m_params[13]; // store current parameters (optional)
    };

    // -----------------------------------------------------------------
    // Echo filter (uses AL_EFFECT_ECHO)
    // -----------------------------------------------------------------
    class AudioEchoFilter : public AudioFilter {
    public:
        AudioEchoFilter();
        ~AudioEchoFilter();

        bool Initialize() override;
        void AttachToSource(SoundSource* source) override;
        void DetachFromSource(SoundSource* source) override;

        void SetDelay(float delay);      // seconds (0.0 – 0.207)
        void SetLRDelay(float lrDelay);  // seconds (0.0 – 0.404)
        void SetDamping(float damping);  // 0.0 – 0.99
        void SetFeedback(float feedback);// 0.0 – 1.0
        void SetSpread(float spread);    // -1.0 – 1.0
    };

    // -----------------------------------------------------------------
    // Low‑pass filter (uses AL_FILTER_LOWPASS)
    // -----------------------------------------------------------------
    class AudioLowPassFilter : public AudioFilter {
    public:
        AudioLowPassFilter();
        ~AudioLowPassFilter();

        bool Initialize() override;
        void AttachToSource(SoundSource* source) override;
        void DetachFromSource(SoundSource* source) override;

        void SetGain(float gain);          // 0.0 – 1.0
        void SetGainHF(float gainHF);      // 0.0 – 1.0
    };

    // -----------------------------------------------------------------
    // High‑pass filter (uses AL_FILTER_HIGHPASS)
    // -----------------------------------------------------------------
    class AudioHighPassFilter : public AudioFilter {
    public:
        AudioHighPassFilter();
        ~AudioHighPassFilter();

        bool Initialize() override;
        void AttachToSource(SoundSource* source) override;
        void DetachFromSource(SoundSource* source) override;

        void SetGain(float gain);          // 0.0 – 1.0
        void SetGainLF(float gainLF);      // 0.0 – 1.0
    };

    // -----------------------------------------------------------------
    // Band‑pass filter (uses AL_FILTER_BANDPASS)
    // -----------------------------------------------------------------
    class AudioBandPassFilter : public AudioFilter {
    public:
        AudioBandPassFilter();
        ~AudioBandPassFilter();

        bool Initialize() override;
        void AttachToSource(SoundSource* source) override;
        void DetachFromSource(SoundSource* source) override;

        void SetGain(float gain);          // 0.0 – 1.0
        void SetGainLF(float gainLF);      // 0.0 – 1.0
        void SetGainHF(float gainHF);      // 0.0 – 1.0
    };

} // namespace USE