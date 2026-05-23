// ============================================================
// Ultimate Source Engine - Texture Loader
// ============================================================
//
// Loads 2D textures from image files (PNG, JPG, TGA, etc.) using stb_image.
// Returns a Texture object suitable for the current rendering backend.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class Texture;

    class TextureLoader {
    public:
        // Load a texture from an image file.
        // Returns a new Texture object (owned by caller) or nullptr on failure.
        static Texture* LoadFromFile(const std::string& filename);
    };

} // namespace USE