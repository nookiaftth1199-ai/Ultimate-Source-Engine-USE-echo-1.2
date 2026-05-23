// ============================================================
// Ultimate Source Engine - Scene Serializer
// ============================================================
//
// Saves and loads scenes to/from JSON files. Handles entities,
// components, and global scene properties.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class Scene;
    class ResourceManager;

    class SceneSerializer {
    public:
        // Save a scene to a JSON file. Returns true on success.
        static bool SaveToFile(const Scene* scene,
                               const std::string& filename,
                               ResourceManager* resourceManager);

        // Load a scene from a JSON file. Returns a new Scene object (caller owns it).
        // If the scene cannot be loaded, returns nullptr.
        static Scene* LoadFromFile(const std::string& filename,
                                   ResourceManager* resourceManager);

    private:
        // Private constructor – all methods static.
        SceneSerializer() = delete;
    };

} // namespace USE