// ============================================================
// Ultimate Source Engine - Model Loader
// ============================================================
//
// Loads 3D models from various formats using Assimp and
// constructs engine‑specific Mesh objects.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Model.h"
#include <string>

namespace USE {

    class IRenderDevice;
    class ResourceManager;

    class ModelLoader {
    public:
        // Load a model from file. Returns a new Model object (caller owns it).
        // The model is built using the provided render device for mesh creation.
        // The resource manager is used for loading textures (materials).
        static Model* LoadFromFile(const std::string& filename,
                                    IRenderDevice* device,
                                    ResourceManager* resourceManager);

    private:
        // Helper functions (static) for processing Assimp data structures
        static bool ProcessNode(void* aiNode, int parentIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static void ProcessMesh(void* aiMesh, void* aiMaterial,
                                int nodeIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static Material* ProcessMaterial(void* aiMaterial,
                                          ResourceManager* resourceManager);
    };

} // namespace USE