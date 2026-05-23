// ============================================================
// Base64.cpp
// ============================================================
#include "Base64.h"
#include <cstring>

static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

namespace USE {
    std::string Base64::Encode(const void* data, size_t length) {
        const unsigned char* bytes = (const unsigned char*)data;
        std::string result;
        for (size_t i = 0; i < length; i += 3) {
            uint32_t block = 0;
            int pad = 0;
            block |= bytes[i] << 16;
            if (i+1 < length) { block |= bytes[i+1] << 8; } else pad++;
            if (i+2 < length) { block |= bytes[i+2]; } else pad++;
            result.push_back(b64chars[(block >> 18) & 0x3F]);
            result.push_back(b64chars[(block >> 12) & 0x3F]);
            if (pad < 2) result.push_back(b64chars[(block >> 6) & 0x3F]); else result.push_back('=');
            if (pad < 1) result.push_back(b64chars[block & 0x3F]); else result.push_back('=');
        }
        return result;
    }
    std::string Base64::Encode(const std::vector<uint8_t>& data) { return Encode(data.data(), data.size()); }
    std::vector<uint8_t> Base64::Decode(const std::string& base64) {
        std::vector<uint8_t> out;
        int val = 0, valb = -8;
        for (unsigned char c : base64) {
            if (c == '=') break;
            int idx;
            if (c >= 'A' && c <= 'Z') idx = c - 'A';
            else if (c >= 'a' && c <= 'z') idx = c - 'a' + 26;
            else if (c >= '0' && c <= '9') idx = c - '0' + 52;
            else if (c == '+') idx = 62;
            else if (c == '/') idx = 63;
            else continue;
            val = (val << 6) | idx;
            valb += 6;
            if (valb >= 0) {
                out.push_back((val >> valb) & 0xFF);
                valb -= 8;
            }
        }
        return out;
    }
}