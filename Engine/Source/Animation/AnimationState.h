// ============================================================
// Ultimate Source Engine - Animation State
//============================================================
//
// Represents the current state of a playing animation clip.
// Holds the current time, speed, loop flag, and weight for blending.
// ============================================================

#pragma once

#include "stdafx.h"
#include "AnimationClip.h"

namespace USE {

    class AnimationState {
    public:
        AnimationState();
        explicit AnimationState(AnimationClip* clip);
        ~AnimationState() = default;

        // Set the animation clip
        void SetClip(AnimationClip* clip) { m_clip = clip; }
        AnimationClip* GetClip() const { return m_clip; }

        // Playback control
        void Play();
        void Pause();
        void Stop();
        void Reset();

        // Update the state (advance time)
        void Update(float deltaTime);

        // Getters
        float GetCurrentTime() const { return m_currentTime; }
        float GetSpeed() const { return m_speed; }
        void  SetSpeed(float speed) { m_speed = speed; }
        bool  IsPlaying() const { return m_playing; }
        bool  IsLooping() const { return m_loop; }
        void  SetLooping(bool loop) { m_loop = loop; }
        float GetWeight() const { return m_weight; }
        void  SetWeight(float weight) { m_weight = weight; }

        // Sample the animation at the current time and output local transforms
        void Sample(Transform* outLocalTransforms, int boneCount) const;

    private:
        AnimationClip* m_clip;
        float          m_currentTime;
        float          m_speed;
        bool           m_playing;
        bool           m_loop;
        float          m_weight;        // blending weight (0-1)
    };

} // namespace USE