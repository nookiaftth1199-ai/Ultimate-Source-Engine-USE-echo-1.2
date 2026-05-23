// ============================================================
// Ultimate Source Engine - Math Utilities Implementation
// ============================================================

#include "stdafx.h"
#include "MathUtils.h"

namespace USE {
namespace MathUtils {

    static bool s_randomSeeded = false;

    void SeedRandom(unsigned int seed)
    {
        srand(seed);
        s_randomSeeded = true;
    }

    float RandomFloat()
    {
        if (!s_randomSeeded) {
            SeedRandom(static_cast<unsigned int>(time(nullptr)));
        }
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    float RandomFloat(float min, float max)
    {
        return min + RandomFloat() * (max - min);
    }

    int RandomInt(int min, int max)
    {
        if (!s_randomSeeded) {
            SeedRandom(static_cast<unsigned int>(time(nullptr)));
        }
        return min + rand() % (max - min + 1);
    }

    float WrapToPi(float angle)
    {
        while (angle > PI) angle -= TAU;
        while (angle < -PI) angle += TAU;
        return angle;
    }

    float WrapTo2Pi(float angle)
    {
        while (angle > TAU) angle -= TAU;
        while (angle < 0) angle += TAU;
        return angle;
    }

    unsigned int NextPowerOfTwo(unsigned int x)
    {
        if (x == 0) return 1;
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }

    // Explicit template instantiations for common types
    template float MinComponent(const Vector2&);
    template float MinComponent(const Vector3&);
    template float MaxComponent(const Vector2&);
    template float MaxComponent(const Vector3&);

} // namespace MathUtils
} // namespace USE