// ============================================================
// Ultimate Source Engine - Audio Filter Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "AudioFilter.h"
#include "SoundSource.h"
#include "Core/Logger.h"

// OpenAL headers (including EFX extensions)
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>  // for EFX tokens

namespace USE {

    // -----------------------------------------------------------------
    // AudioFilter base class
    // -----------------------------------------------------------------
    AudioFilter::AudioFilter()
        : m_filterId(0)
        , m_effectId(0)
        , m_slotId(0)
        , m_enabled(true)
    {
    }

    AudioFilter::~AudioFilter()
    {
        if (m_filterId) alDeleteFilters(1, &m_filterId);
        if (m_effectId) alDeleteEffects(1, &m_effectId);
        if (m_slotId)   alDeleteAuxiliaryEffectSlots(1, &m_slotId);
    }

    void AudioFilter::SetEnabled(bool enabled)
    {
        m_enabled = enabled;
        // The actual enabling/disabling would happen when attaching/detaching.
        // For now, we just store the flag.
    }

    // -----------------------------------------------------------------
    // Helper: check if EFX is available
    // -----------------------------------------------------------------
    static bool CheckEFXAvailable()
    {
        ALCdevice* device = alcGetContextsDevice(alcGetCurrentContext());
        if (!device) return false;
        ALCboolean efxPresent = alcIsExtensionPresent(device, "ALC_EXT_EFX");
        return efxPresent == ALC_TRUE;
    }

    // -----------------------------------------------------------------
    // Reverb filter
    // -----------------------------------------------------------------
    AudioReverbFilter::AudioReverbFilter()
    {
        // Initialize parameter array (optional)
    }

    AudioReverbFilter::~AudioReverbFilter()
    {
    }

    bool AudioReverbFilter::Initialize()
    {
        if (!CheckEFXAvailable()) {
            USE_LOG_ERROR("AudioReverbFilter: EFX not available");
            return false;
        }

        // Generate effect object
        alGenEffects(1, &m_effectId);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioReverbFilter: Failed to generate effect");
            return false;
        }

        // Set effect type to reverb
        alEffecti(m_effectId, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioReverbFilter: Failed to set effect type to reverb");
            alDeleteEffects(1, &m_effectId);
            m_effectId = 0;
            return false;
        }

        // Generate auxiliary effect slot
        alGenAuxiliaryEffectSlots(1, &m_slotId);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioReverbFilter: Failed to generate effect slot");
            alDeleteEffects(1, &m_effectId);
            m_effectId = 0;
            return false;
        }

        // Attach effect to slot
        alAuxiliaryEffectSloti(m_slotId, AL_EFFECTSLOT_EFFECT, (ALint)m_effectId);
        if (alGetError() != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioReverbFilter: Failed to attach effect to slot");
            alDeleteAuxiliaryEffectSlots(1, &m_slotId);
            alDeleteEffects(1, &m_effectId);
            m_slotId = 0;
            m_effectId = 0;
            return false;
        }

        return true;
    }

    void AudioReverbFilter::AttachToSource(SoundSource* source)
    {
        if (!source || !m_enabled || !m_slotId) return;
        alSource3i(source->GetHandle(), AL_AUXILIARY_SEND_FILTER, (ALint)m_slotId, 0, AL_FILTER_NULL);
    }

    void AudioReverbFilter::DetachFromSource(SoundSource* source)
    {
        if (!source || !m_slotId) return;
        alSource3i(source->GetHandle(), AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
    }

    // Parameter setters (simplified – they just call alEffectf)
    #define SET_REVERB_PARAM(param, value) \
        if (m_effectId) { alEffectf(m_effectId, param, value); }

    void AudioReverbFilter::SetDensity(float density) { SET_REVERB_PARAM(AL_REVERB_DENSITY, density); }
    void AudioReverbFilter::SetDiffusion(float diffusion) { SET_REVERB_PARAM(AL_REVERB_DIFFUSION, diffusion); }
    void AudioReverbFilter::SetGain(float gain) { SET_REVERB_PARAM(AL_REVERB_GAIN, gain); }
    void AudioReverbFilter::SetGainHF(float gainHF) { SET_REVERB_PARAM(AL_REVERB_GAINHF, gainHF); }
    void AudioReverbFilter::SetDecayTime(float time) { SET_REVERB_PARAM(AL_REVERB_DECAY_TIME, time); }
    void AudioReverbFilter::SetDecayHFRatio(float ratio) { SET_REVERB_PARAM(AL_REVERB_DECAY_HFRATIO, ratio); }
    void AudioReverbFilter::SetReflectionsGain(float gain) { SET_REVERB_PARAM(AL_REVERB_REFLECTIONS_GAIN, gain); }
    void AudioReverbFilter::SetReflectionsDelay(float delay) { SET_REVERB_PARAM(AL_REVERB_REFLECTIONS_DELAY, delay); }
    void AudioReverbFilter::setLateReverbGain(float gain) { SET_REVERB_PARAM(AL_REVERB_LATE_REVERB_GAIN, gain); }
    void AudioReverbFilter::setLateReverbDelay(float delay) { SET_REVERB_PARAM(AL_REVERB_LATE_REVERB_DELAY, delay); }
    void AudioReverbFilter::SetAirAbsorptionGainHF(float gain) { SET_REVERB_PARAM(AL_REVERB_AIR_ABSORPTION_GAINHF, gain); }
    void AudioReverbFilter::SetRoomRolloffFactor(float factor) { SET_REVERB_PARAM(AL_REVERB_ROOM_ROLLOFF_FACTOR, factor); }

    // -----------------------------------------------------------------
    // Echo filter
    // -----------------------------------------------------------------
    AudioEchoFilter::AudioEchoFilter() {}
    AudioEchoFilter::~AudioEchoFilter() {}

    bool AudioEchoFilter::Initialize()
    {
        if (!CheckEFXAvailable()) return false;

        alGenEffects(1, &m_effectId);
        if (alGetError() != AL_NO_ERROR) return false;
        alEffecti(m_effectId, AL_EFFECT_TYPE, AL_EFFECT_ECHO);
        if (alGetError() != AL_NO_ERROR) {
            alDeleteEffects(1, &m_effectId);
            return false;
        }

        alGenAuxiliaryEffectSlots(1, &m_slotId);
        if (alGetError() != AL_NO_ERROR) {
            alDeleteEffects(1, &m_effectId);
            return false;
        }

        alAuxiliaryEffectSloti(m_slotId, AL_EFFECTSLOT_EFFECT, (ALint)m_effectId);
        return true;
    }

    void AudioEchoFilter::AttachToSource(SoundSource* source)
    {
        if (!source || !m_enabled || !m_slotId) return;
        alSource3i(source->GetHandle(), AL_AUXILIARY_SEND_FILTER, (ALint)m_slotId, 0, AL_FILTER_NULL);
    }

    void AudioEchoFilter::DetachFromSource(SoundSource* source)
    {
        if (!source) return;
        alSource3i(source->GetHandle(), AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
    }

    void AudioEchoFilter::SetDelay(float delay) { alEffectf(m_effectId, AL_ECHO_DELAY, delay); }
    void AudioEchoFilter::SetLRDelay(float lrDelay) { alEffectf(m_effectId, AL_ECHO_LRDELAY, lrDelay); }
    void AudioEchoFilter::SetDamping(float damping) { alEffectf(m_effectId, AL_ECHO_DAMPING, damping); }
    void AudioEchoFilter::SetFeedback(float feedback) { alEffectf(m_effectId, AL_ECHO_FEEDBACK, feedback); }
    void AudioEchoFilter::SetSpread(float spread) { alEffectf(m_effectId, AL_ECHO_SPREAD, spread); }

    // -----------------------------------------------------------------
    // Low‑pass filter (direct filter, not send)
    // -----------------------------------------------------------------
    AudioLowPassFilter::AudioLowPassFilter() {}
    AudioLowPassFilter::~AudioLowPassFilter() {}

    bool AudioLowPassFilter::Initialize()
    {
        if (!CheckEFXAvailable()) return false;

        alGenFilters(1, &m_filterId);
        if (alGetError() != AL_NO_ERROR) return false;
        alFilteri(m_filterId, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        return alGetError() == AL_NO_ERROR;
    }

    void AudioLowPassFilter::AttachToSource(SoundSource* source)
    {
        if (!source || !m_enabled || !m_filterId) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, (ALint)m_filterId);
    }

    void AudioLowPassFilter::DetachFromSource(SoundSource* source)
    {
        if (!source) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, AL_FILTER_NULL);
    }

    void AudioLowPassFilter::SetGain(float gain) { alFilterf(m_filterId, AL_LOWPASS_GAIN, gain); }
    void AudioLowPassFilter::SetGainHF(float gainHF) { alFilterf(m_filterId, AL_LOWPASS_GAINHF, gainHF); }

    // -----------------------------------------------------------------
    // High‑pass filter (direct filter)
    // -----------------------------------------------------------------
    AudioHighPassFilter::AudioHighPassFilter() {}
    AudioHighPassFilter::~AudioHighPassFilter() {}

    bool AudioHighPassFilter::Initialize()
    {
        if (!CheckEFXAvailable()) return false;
        alGenFilters(1, &m_filterId);
        if (alGetError() != AL_NO_ERROR) return false;
        alFilteri(m_filterId, AL_FILTER_TYPE, AL_FILTER_HIGHPASS);
        return alGetError() == AL_NO_ERROR;
    }

    void AudioHighPassFilter::AttachToSource(SoundSource* source)
    {
        if (!source || !m_enabled || !m_filterId) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, (ALint)m_filterId);
    }

    void AudioHighPassFilter::DetachFromSource(SoundSource* source)
    {
        if (!source) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, AL_FILTER_NULL);
    }

    void AudioHighPassFilter::SetGain(float gain) { alFilterf(m_filterId, AL_HIGHPASS_GAIN, gain); }
    void AudioHighPassFilter::SetGainLF(float gainLF) { alFilterf(m_filterId, AL_HIGHPASS_GAINLF, gainLF); }

    // -----------------------------------------------------------------
    // Band‑pass filter (direct filter)
    // -----------------------------------------------------------------
    AudioBandPassFilter::AudioBandPassFilter() {}
    AudioBandPassFilter::~AudioBandPassFilter() {}

    bool AudioBandPassFilter::Initialize()
    {
        if (!CheckEFXAvailable()) return false;
        alGenFilters(1, &m_filterId);
        if (alGetError() != AL_NO_ERROR) return false;
        alFilteri(m_filterId, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
        return alGetError() == AL_NO_ERROR;
    }

    void AudioBandPassFilter::AttachToSource(SoundSource* source)
    {
        if (!source || !m_enabled || !m_filterId) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, (ALint)m_filterId);
    }

    void AudioBandPassFilter::DetachFromSource(SoundSource* source)
    {
        if (!source) return;
        alSourcei(source->GetHandle(), AL_DIRECT_FILTER, AL_FILTER_NULL);
    }

    void AudioBandPassFilter::SetGain(float gain) { alFilterf(m_filterId, AL_BANDPASS_GAIN, gain); }
    void AudioBandPassFilter::SetGainLF(float gainLF) { alFilterf(m_filterId, AL_BANDPASS_GAINLF, gainLF); }
    void AudioBandPassFilter::SetGainHF(float gainHF) { alFilterf(m_filterId, AL_BANDPASS_GAINHF, gainHF); }

} // namespace USE