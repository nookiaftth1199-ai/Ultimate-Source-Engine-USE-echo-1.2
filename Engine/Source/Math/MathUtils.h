// ============================================================
// Ultimate Source Engine - Math Utilities
// ============================================================
//
// Collection of common mathematical functions not tied to specific classes.
// ============================================================

#pragma once

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace USE {
namespace MathUtils {

    // Constants
    const float PI = 3.14159265358979323846f;
    const float HALF_PI = 1.57079632679489661923f;
    const float TAU = 6.28318530717958647692f;
    const float DEG_TO_RAD = PI / 180.0f;
    const float RAD_TO_DEG = 180.0f / PI;
    const float EPSILON = 1e-6f;

    // Basic math functions
    inline float Abs(float x) { return x < 0 ? -x : x; }
    inline float Min(float a, float b) { return a < b ? a : b; }
    inline float Max(float a, float b) { return a > b ? a : b; }
    inline float Clamp(float x, float minVal, float maxVal) {
        return x < minVal ? minVal : (x > maxVal ? maxVal : x);
    }
    inline float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
    inline float SmoothStep(float a, float b, float t) {
        t = Clamp((t - a) / (b - a), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    // Angle conversion
    inline float Radians(float degrees) { return degrees * DEG_TO_RAD; }
    inline float Degrees(float radians) { return radians * RAD_TO_DEG; }

    // Wrap angle to [-PI, PI] or [0, 2PI]
    inline float WrapToPi(float angle);
    inline float WrapTo2Pi(float angle);

    // Random number generation (simple, not thread-safe)
    void SeedRandom(unsigned int seed);
    float RandomFloat();            // [0,1)
    float RandomFloat(float min, float max);
    int RandomInt(int min, int max); // inclusive

    // Vector utilities (2D, 3D – but keep generic)
    template<typename T>
    inline T MinComponent(const T& v);
    template<typename T>
    inline T MaxComponent(const T& v);

    // Check if two floats are approximately equal
    inline bool Approximately(float a, float b, float epsilon = EPSILON) {
        return Abs(a - b) <= epsilon;
    }

    // Next power of two
    inline unsigned int NextPowerOfTwo(unsigned int x);

} // namespace MathUtils
} // namespace USE