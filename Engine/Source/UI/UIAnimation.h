// ============================================================
// Ultimate Source Engine - UI Animation
//============================================================
//
// Provides a framework for animating UI element properties
// (position, size, color, opacity, etc.) over time.
// Supports easing, looping, and completion callbacks.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector2.h"
#include "Math/Color.h"
#include <functional>

namespace USE {

    // Easing function types
    enum class EasingType {
        Linear,
        QuadraticIn, QuadraticOut, QuadraticInOut,
        CubicIn, CubicOut, CubicInOut,
        SineIn, SineOut, SineInOut,
        ExpoIn, ExpoOut, ExpoInOut,
        ElasticIn, ElasticOut, ElasticInOut,
        BounceIn, BounceOut, BounceInOut
    };

    // Base class for all UI animations
    class UIAnimation {
    public:
        UIAnimation();
        virtual ~UIAnimation() = default;

        // Configuration
        void SetDuration(float seconds);
        void SetLoopCount(int loops);          // -1 = infinite
        void SetDelay(float seconds);
        void SetEasing(EasingType type);
        void SetOnComplete(std::function<void()> callback);

        // Control
        void Play();
        void Pause();
        void Stop();
        void Seek(float time);
        bool IsPlaying() const { return m_state == State::Playing; }

        // Called by the UI system each frame
        void Update(float deltaTime);

    protected:
        enum class State { Stopped, Playing, Paused, Finished };
        State m_state;
        float m_duration;
        float m_elapsed;
        float m_delay;
        int   m_loopCount;          // remaining loops (0 = finished, -1 = infinite)
        int   m_originalLoops;
        float m_delayRemaining;

        EasingType m_easingType;
        std::function<void()> m_onComplete;

        // Convert elapsed time to eased normalized value [0,1]
        float ComputeEasedT() const;

        // Apply the animation at the given normalized time t
        virtual void Apply(float t) = 0;
    };

    // -----------------------------------------------------------------
    // Concrete animation for a float property (e.g., opacity, rotation)
    // -----------------------------------------------------------------
    class UIFloatAnimation : public UIAnimation {
    public:
        using Setter = std::function<void(float)>;
        UIFloatAnimation(Setter setter, float start, float end);

    protected:
        void Apply(float t) override;

    private:
        Setter m_setter;
        float m_start;
        float m_end;
    };

    // -----------------------------------------------------------------
    // Concrete animation for a Vector2 property (e.g., position, size)
    // -----------------------------------------------------------------
    class UIVector2Animation : public UIAnimation {
    public:
        using Setter = std::function<void(const Vector2&)>;
        UIVector2Animation(Setter setter, const Vector2& start, const Vector2& end);

    protected:
        void Apply(float t) override;

    private:
        Setter m_setter;
        Vector2 m_start;
        Vector2 m_end;
    };

    // -----------------------------------------------------------------
    // Concrete animation for a Color property (e.g., text color, background)
    // -----------------------------------------------------------------
    class UIColorAnimation : public UIAnimation {
    public:
        using Setter = std::function<void(const Color&)>;
        UIColorAnimation(Setter setter, const Color& start, const Color& end);

    protected:
        void Apply(float t) override;

    private:
        Setter m_setter;
        Color m_start;
        Color m_end;
    };

} // namespace USE