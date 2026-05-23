// ============================================================
// STBImageWrapper.cpp
// ============================================================
#include "STBImageWrapper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace USE {
    unsigned char* STBImageWrapper::Load(const char* filename, int* width, int* height, int* channels, int desired_channels) {
        return stbi_load(filename, width, height, channels, desired_channels);
    }

    void STBImageWrapper::Free(unsigned char* data) {
        stbi_image_free(data);
    }
}