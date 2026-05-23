// ============================================================
// STBImageWrapper.h
// ============================================================
#pragma once
#include <cstdint>

namespace USE {
    class STBImageWrapper {
    public:
        static unsigned char* Load(const char* filename, int* width, int* height, int* channels, int desired_channels);
        static void Free(unsigned char* data);
    };
}