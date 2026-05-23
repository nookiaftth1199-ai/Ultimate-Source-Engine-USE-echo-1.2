// ============================================================
// Ultimate Source Engine - Texture Loader Implementation
// ============================================================

#include "stdafx.h"
#include "TextureLoader.h"
#include "Renderer/Texture.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

// stb_image for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace USE {

    Texture* TextureLoader::LoadFromFile(const std::string& filename)
    {
        // Use FileSystem to resolve the path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("TextureLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("TextureLoader: Texture file not found: %s", filename.c_str());
            return nullptr;
        }

        // Load image data via stb_image
        int width, height, channels;
        stbi_uc* pixels = stbi_load(resolved.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            USE_LOG_ERROR("TextureLoader: Failed to load image: %s (stb_image error)", resolved.c_str());
            return nullptr;
        }

        // Determine texture format (always RGBA8 due to STBI_rgb_alpha)
        TextureFormat format = TextureFormat::RGBA8_UNORM;

        // Create a texture object via the backend factory
        Texture* texture = Texture::Create();
        if (!texture) {
            USE_LOG_ERROR("TextureLoader: Failed to create texture object (backend unavailable?)");
            stbi_image_free(pixels);
            return nullptr;
        }

        // Initialize texture with image data
        bool success = texture->Create(width, height, format, pixels);
        stbi_image_free(pixels);

        if (!success) {
            USE_LOG_ERROR("TextureLoader: Failed to initialize texture with image data: %s", filename.c_str());
            delete texture;
            return nullptr;
        }

        USE_LOG_INFO("TextureLoader: Loaded texture: %s (%dx%d)", filename.c_str(), width, height);
        return texture;
    }

} // namespace USE#pragma