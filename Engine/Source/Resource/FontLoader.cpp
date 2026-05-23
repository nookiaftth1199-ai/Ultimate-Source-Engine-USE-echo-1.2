// ============================================================
// Ultimate Source Engine - Font Loader Implementation
// ============================================================

#include "stdafx.h"
#include "FontLoader.h"
#include "UI/Font.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    Font* FontLoader::LoadFromFile(const std::string& filename)
    {
        // Use FileSystem to resolve the path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("FontLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("FontLoader: Font file not found: %s", filename.c_str());
            return nullptr;
        }

        // Open the font file
        auto file = fs->OpenFile(resolved, FILE_READ | FILE_BINARY);
        if (!file) {
            USE_LOG_ERROR("FontLoader: Could not open font file: %s", resolved.c_str());
            return nullptr;
        }

        // Read entire file into memory
        size_t fileSize = file->GetSize();
        std::vector<uint8_t> buffer(fileSize);
        if (file->Read(buffer.data(), 1, fileSize) != fileSize) {
            USE_LOG_ERROR("FontLoader: Failed to read font file: %s", resolved.c_str());
            file->Close();
            return nullptr;
        }
        file->Close();

        // Create a new Font object
        Font* font = new Font();

        // Load from memory (the Font class must have this method)
        if (!font->LoadFromMemory(buffer.data(), buffer.size())) {
            USE_LOG_ERROR("FontLoader: Failed to parse font: %s", filename.c_str());
            delete font;
            return nullptr;
        }

        USE_LOG_INFO("FontLoader: Loaded font: %s", filename.c_str());
        return font;
    }

} // namespace USE