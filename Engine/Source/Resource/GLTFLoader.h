// ============================================================
// Ultimate Source Engine - glTF Loader
// ============================================================
//
// Loads 3D models from glTF 2.0 files using the cgltf library.
// Creates engine‑specific Mesh objects and materials.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Model.h"
#include <string>

namespace USE {

    class IRenderDevice;
    class ResourceManager;

    class GLTFLoader {
    public:
        // Load a glTF model from file. Returns a new Model object (caller owns it).
        static Model* LoadFromFile(const std::string& filename,
                                    IRenderDevice* device,
                                    ResourceManager* resourceManager);

    private:
        // Helper functions (static) for processing glTF data structures
        static bool ProcessNode(void* gltfNode, int parentIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static void ProcessMesh(void* gltfMesh, int meshIndex,
                                int nodeIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static void ProcessPrimitive(void* gltfPrimitive,
                                      IRenderDevice* device,
                                      Model* outModel,
                                      std::vector<uint32_t>& meshIndices);
        static Material* ProcessMaterial(void* gltfMaterial,
                                          ResourceManager* resourceManager);
    };

} // namespace USE