// ============================================================
// Ultimate Source Engine - Scene Loader
// ============================================================
//
// Loads a scene from a JSON file, creating entities and components
// as defined in the file. Uses the resource manager to load assets.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

namespace USE {

    class Scene;
    class ResourceManager;
    class EntityManager;

    class SceneLoader {
    public:
        // Load a scene from a JSON file. Returns a new Scene object (caller owns it).
        // If the scene cannot be loaded, returns nullptr.
        static Scene* LoadFromFile(const std::string& filename,
                                   ResourceManager* resourceManager);

        // Load a scene into an existing world (without creating a Scene object).
        // Entities are added to the given entity manager.
        static bool LoadIntoWorld(const std::string& filename,
                                  ResourceManager* resourceManager,
                                  EntityManager* entityManager);

    private:
        // Private constructor – all methods static.
        SceneLoader() = delete;
    };

} // namespace USE