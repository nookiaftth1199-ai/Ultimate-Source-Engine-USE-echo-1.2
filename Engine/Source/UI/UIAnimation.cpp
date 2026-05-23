// ============================================================
// Ultimate Source Engine - UI Animation Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIAnimation.h"
#include "Math/MathUtils.h"
#include <cmath>

namespace USE {

    // -----------------------------------------------------------------
    // Easing function helpers (static)
    // -----------------------------------------------------------------
    static float EaseLinear(float t) { return t; }

    static float EaseQuadraticIn(float t) { return t * t; }
    static float EaseQuadraticOut(float t) { return t * (2.0f - t); }
    static float EaseQuadraticInOut(float t) {
        t *= 2.0f;
        if (t < 1.0f) return 0.5f * t * t;
        t -= 1.0f;
        return -0.5f * (t * (t - 2.0f) - 1.0f);
    }

    static float EaseCubicIn(float t) { return t * t * t; }
    static float EaseCubicOut(float t) { t -= 1.0f; return t * t * t + 1.0f; }
    static float EaseCubicInOut(float t) {
        t *= 2.0f;
        if (t < 1.0f) return 0.5f * t * t * t;
        t -= 2.0f;
        return 0.5f * (t * t * t + 2.0f);
    }

    static float EaseSineIn(float t) { return 1.0f - cosf(t * MathUtils::HALF_PI); }
    static float EaseSineOut(float t) { return sinf(t * MathUtils::HALF_PI); }
    static float EaseSineInOut(float t) { return -0.5f * (cosf(t * MathUtils::PI) - 1.0f); }

    // More easing functions can be added as needed...

    // -----------------------------------------------------------------
    // UIAnimation
    // -----------------------------------------------------------------
    UIAnimation::UIAnimation()
        : m_state(State::Stopped)
        , m_duration(1.0f)
        , m_elapsed(0.0f)
        , m_delay(0.0f)
        , m_loopCount(0)
        , m_originalLoops(0)
        , m_delayRemaining(0.0f)
        , m_easingType(EasingType::Linear)
    {
    }

    void UIAnimation::SetDuration(float seconds) {
        m_duration = seconds > 0.0f ? seconds : 0.001f;
    }

    void UIAnimation::SetLoopCount(int loops) {
        m_originalLoops = loops;
        m_loopCount = loops;
    }

    void UIAnimation::SetDelay(float seconds) {
        m_delay = seconds;
    }

    void UIAnimation::SetEasing(EasingType type) {
        m_easingType = type;
    }

    void UIAnimation::SetOnComplete(std::function<void()> callback) {
        m_onComplete = callback;
    }

    void UIAnimation::Play() {
        if (m_state == State::Playing) return;
        m_state = State::Playing;
        m_elapsed = 0.0f;
        m_delayRemaining = m_delay;
        m_loopCount = m_originalLoops;
    }

    void UIAnimation::Pause() {
        if (m_state == State::Playing) {
            m_state = State::Paused;
        }
    }

    void UIAnimation::Stop() {
        m_state = State::Stopped;
        m_elapsed = 0.0f;
        m_delayRemaining = 0.0f;
    }

    void UIAnimation::Seek(float time) {
        m_elapsed = time;
    }

    void UIAnimation::Update(float deltaTime) {
        if (m_state != State::Playing) return;

        // Handle initial delay
        if (m_delayRemaining > 0.0f) {
            m_delayRemaining -= deltaTime;
            if (m_delayRemaining > 0.0f) return;
            // Delay finished; adjust deltaTime for the remainder
            deltaTime = -m_delayRemaining;
            m_delayRemaining = 0.0f;
        }

        m_elapsed += deltaTime;

        float t = m_elapsed / m_duration;

        if (t >= 1.0f) {
            // End of one loop
            if (m_loopCount != 0) {
                if (m_loopCount > 0) m_loopCount--;
                m_elapsed = 0.0f;
                Apply(1.0f); // Apply final frame of this loop
                if (m_loopCount == 0) {
                    m_state = State::Finished;
                    if (m_onComplete) m_onComplete();
                }
            } else {
                m_state = State::Finished;
                Apply(1.0f);
                if (m_onComplete) m_onComplete();
            }
        } else {
            Apply(ComputeEasedT());
        }
    }

    float UIAnimation::ComputeEasedT() const {
        float t = m_elapsed / m_duration;
        t = MathUtils::Clamp(t, 0.0f, 1.0f);
        switch (m_easingType) {
            case EasingType::Linear:           return EaseLinear(t);
            case EasingType::QuadraticIn:      return EaseQuadraticIn(t);
            case EasingType::QuadraticOut:     return EaseQuadraticOut(t);
            case EasingType::QuadraticInOut:   return EaseQuadraticInOut(t);
            case EasingType::CubicIn:          return EaseCubicIn(t);
            case EasingType::CubicOut:         return EaseCubicOut(t);
            case EasingType::CubicInOut:       return EaseCubicInOut(t);
            case EasingType::SineIn:            return EaseSineIn(t);
            case EasingType::SineOut:           return EaseSineOut(t);
            case EasingType::SineInOut:         return EaseSineInOut(t);
            // For brevity, other easings would be added similarly
            default: return t;
        }
    }

    // -----------------------------------------------------------------
    // UIFloatAnimation
    // -----------------------------------------------------------------
    UIFloatAnimation::UIFloatAnimation(Setter setter, float start, float end)
        : m_setter(setter), m_start(start), m_end(end)
    {
    }

    void UIFloatAnimation::Apply(float t) {
        if (m_setter) {
            float value = m_start + (m_end - m_start) * t;
            m_setter(value);
        }
    }

    // -----------------------------------------------------------------
    // UIVector2Animation
    // -----------------------------------------------------------------
    UIVector2Animation::UIVector2Animation(Setter setter, const Vector2& start, const Vector2& end)
        : m_setter(setter), m_start(start), m_end(end)
    {
    }

    void UIVector2Animation::Apply(float t) {
        if (m_setter) {
            Vector2 value = m_start + (m_end - m_start) * t;
            m_setter(value);
        }
    }

    // -----------------------------------------------------------------
    // UIColorAnimation
    // -----------------------------------------------------------------
    UIColorAnimation::UIColorAnimation(Setter setter, const Color& start, const Color& end)
        : m_setter(setter), m_start(start), m_end(end)
    {
    }

    void UIColorAnimation::Apply(float t) {
        if (m_setter) {
            // Linear interpolation of each component
            Color value;
            value.r = m_start.r + (m_end.r - m_start.r) * t;
            value.g = m_start.g + (m_end.g - m_start.g) * t;
            value.b = m_start.b + (m_end.b - m_start.b) * t;
            value.a = m_start.a + (m_end.a - m_start.a) * t;
            m_setter(value);
        }
    }

} // namespace USE