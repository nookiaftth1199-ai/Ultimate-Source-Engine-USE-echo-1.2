// ============================================================
// Ultimate Source Engine - Resource Loader Interface
// ============================================================
//
// Base interface for custom resource loaders.
// Extend this to support new file formats or resource types.
// ============================================================

#pragma once

#include <string>

namespace USE {

    class ResourceManager;

    class IResourceLoader {
    public:
        virtual ~IResourceLoader() = default;

        // Load a resource from the given path using the resource manager.
        // Returns true on success.
        virtual bool Load(const std::string& path, ResourceManager* manager) = 0;

        // Unload the resource (free memory, GPU resources, etc.)
        virtual void Unload() = 0;

        // Get the type of resource this loader handles (e.g., "Texture", "Model")
        virtual const char* GetResourceType() const = 0;
    };

} // namespace USE