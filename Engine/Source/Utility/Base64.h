// ============================================================
// Base64.h
// ============================================================
#pragma once
#include <string>
#include <vector>

namespace USE {
    class Base64 {
    public:
        static std::string Encode(const void* data, size_t length);
        static std::string Encode(const std::vector<uint8_t>& data);
        static std::vector<uint8_t> Decode(const std::string& base64);
    };
}