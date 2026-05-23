// ============================================================
// Ultimate Source Engine - Random Number Generator
// ============================================================
//
// Provides a simple, seedable random number generator.
// Uses a linear congruential generator (LCG) for speed and simplicity.
// ============================================================

#pragma once

#include <cstdint>

namespace USE {

    class Random {
    public:
        // Constructor with optional seed (defaults to current time)
        Random(uint32_t seed = 0);

        // Re-seed the generator
        void Seed(uint32_t seed);

        // Generate random integer in [0, 2^31-1)
        uint32_t NextUInt();

        // Generate random integer in [0, max) (max exclusive)
        uint32_t NextUInt(uint32_t max);

        // Generate random integer in [min, max] (inclusive)
        int NextInt(int min, int max);

        // Generate random float in [0, 1)
        float NextFloat();

        // Generate random float in [min, max)
        float NextFloat(float min, float max);

        // Generate random double in [0, 1)
        double NextDouble();

        // Generate random double in [min, max)
        double NextDouble(double min, double max);

        // Generate random boolean (50/50 chance)
        bool NextBool();

        // Generate random byte (0-255)
        uint8_t NextByte();

        // Fill a buffer with random bytes
        void NextBytes(void* buffer, size_t count);

    private:
        uint32_t m_state;
    };

} // namespace USE