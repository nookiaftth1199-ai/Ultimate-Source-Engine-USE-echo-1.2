// ============================================================
// CRC32.h
// ============================================================
#pragma once
#include <cstdint>
#include <vector>

namespace USE {
    class CRC32 {
    public:
        static uint32_t Calculate(const void* data, size_t length);
        static uint32_t Calculate(const std::vector<uint8_t>& data);
        static uint32_t Calculate(const char* str);
    };
}