// ============================================================
// Ultimate Source Engine - Random Number Generator Implementation
// ============================================================

#include "stdafx.h"
#include "Random.h"
#include <ctime>

namespace USE {

    // Constants for LCG (same as glibc's rand())
    static const uint32_t LCG_MULTIPLIER = 1103515245;
    static const uint32_t LCG_INCREMENT = 12345;
    static const uint32_t LCG_MODULUS = 0x7fffffff; // 2^31 - 1

    Random::Random(uint32_t seed)
    {
        if (seed == 0) {
            seed = static_cast<uint32_t>(time(nullptr));
        }
        Seed(seed);
    }

    void Random::Seed(uint32_t seed)
    {
        m_state = seed;
    }

    uint32_t Random::NextUInt()
    {
        m_state = (m_state * LCG_MULTIPLIER + LCG_INCREMENT) & LCG_MODULUS;
        return m_state;
    }

    uint32_t Random::NextUInt(uint32_t max)
    {
        return NextUInt() % max;
    }

    int Random::NextInt(int min, int max)
    {
        return min + static_cast<int>(NextUInt() % (max - min + 1));
    }

    float Random::NextFloat()
    {
        return static_cast<float>(NextUInt()) / static_cast<float>(LCG_MODULUS);
    }

    float Random::NextFloat(float min, float max)
    {
        return min + NextFloat() * (max - min);
    }

    double Random::NextDouble()
    {
        return static_cast<double>(NextUInt()) / static_cast<double>(LCG_MODULUS);
    }

    double Random::NextDouble(double min, double max)
    {
        return min + NextDouble() * (max - min);
    }

    bool Random::NextBool()
    {
        return (NextUInt() & 1) != 0;
    }

    uint8_t Random::NextByte()
    {
        return static_cast<uint8_t>(NextUInt() & 0xFF);
    }

    void Random::NextBytes(void* buffer, size_t count)
    {
        uint8_t* bytes = static_cast<uint8_t*>(buffer);
        for (size_t i = 0; i < count; ++i) {
            bytes[i] = NextByte();
        }
    }

} // namespace USE