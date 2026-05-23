// ============================================================
// MD5.h
// ============================================================
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace USE {
    class MD5 {
    public:
        static std::string Calculate(const void* data, size_t length);
        static std::string Calculate(const std::vector<uint8_t>& data);
        static std::string Calculate(const std::string& str);
    };
}