// ============================================================
// Ultimate Source Engine - Font Loader
// ============================================================
//
// Loads TrueType/OpenType font files from disk and creates
// Font objects for text rendering.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class Font;

    class FontLoader {
    public:
        // Load a font from a TTF/OTF file. Returns a new Font object
        // (owned by caller) or nullptr on failure.
        static Font* LoadFromFile(const std::string& filename);
    };

} // namespace USE