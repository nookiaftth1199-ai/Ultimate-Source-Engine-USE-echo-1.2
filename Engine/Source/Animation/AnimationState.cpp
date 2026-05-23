// ============================================================
// Ultimate Source Engine - Animation State Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "AnimationState.h"

namespace USE {

    AnimationState::AnimationState()
        : m_clip(nullptr)
        , m_currentTime(0.0f)
        , m_speed(1.0f)
        , m_playing(false)
        , m_loop(false)
        , m_weight(1.0f)
    {
    }

    AnimationState::AnimationState(AnimationClip* clip)
        : m_clip(clip)
        , m_currentTime(0.0f)
        , m_speed(1.0f)
        , m_playing(false)
        , m_loop(false)
        , m_weight(1.0f)
    {
    }

    void AnimationState::Play()
    {
        m_playing = true;
    }

    void AnimationState::Pause()
    {
        m_playing = false;
    }

    void AnimationState::Stop()
    {
        m_playing = false;
        m_currentTime = 0.0f;
    }

    void AnimationState::Reset()
    {
        m_currentTime = 0.0f;
    }

    void AnimationState::Update(float deltaTime)
    {
        if (!m_playing || !m_clip) return;

        m_currentTime += deltaTime * m_speed;

        if (m_loop) {
            float dur = m_clip->GetDuration();
            if (dur > 0) {
                m_currentTime = fmodf(m_currentTime, dur);
            }
        } else {
            float dur = m_clip->GetDuration();
            if (m_currentTime > dur) {
                m_currentTime = dur;
                m_playing = false; // auto‑stop when finished
            }
        }
    }

    void AnimationState::Sample(Transform* outLocalTransforms, int boneCount) const
    {
        if (m_clip) {
            m_clip->Sample(m_currentTime, m_loop, outLocalTransforms, boneCount);
        } else {
            // No clip: fill with identity
            for (int i = 0; i < boneCount; ++i) {
                outLocalTransforms[i] = Transform();
            }
        }
    }

} // namespace USE