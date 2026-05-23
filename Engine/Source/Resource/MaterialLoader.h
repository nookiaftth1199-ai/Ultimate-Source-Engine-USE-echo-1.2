// ============================================================
// Ultimate Source Engine - Material Loader
// ============================================================
//
// Loads material definitions from JSON files and creates
// Material objects with shaders, textures, and uniform values.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/Material.h"
#include <string>
#include <memory>

namespace USE {

    class ResourceManager;

    class MaterialLoader {
    public:
        // Load a material from a JSON file. Returns a Material object (owned by caller).
        static Material* LoadFromFile(const std::string& filename,
                                      ResourceManager* resourceManager);

    private:
        // Helper to parse uniform values from JSON
        static bool ParseUniforms(Material* material, const class nlohmann::json& j);
    };

} // namespace USE