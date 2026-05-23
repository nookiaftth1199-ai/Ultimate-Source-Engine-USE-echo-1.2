// ============================================================
// Ultimate Source Engine - FBX Loader
// ============================================================
//
// Loads 3D models from Autodesk FBX files using the FBX SDK.
// Creates engine‑specific Mesh objects and materials.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Model.h"
#include <fbxsdk.h>
#include <string>

namespace USE {

    class IRenderDevice;
    class ResourceManager;

    class FBXLoader {
    public:
        // Load an FBX model from file. Returns a new Model object (caller owns it).
        static Model* LoadFromFile(const std::string& filename,
                                   IRenderDevice* device,
                                   ResourceManager* resourceManager);

    private:
        // Helper functions (static) for processing FBX data structures
        static bool ProcessNode(FbxNode* node, int parentIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static void ProcessMesh(FbxMesh* mesh, FbxNode* node,
                                int nodeIndex,
                                IRenderDevice* device,
                                ResourceManager* resourceManager,
                                Model* outModel);
        static void ProcessMaterial(FbxSurfaceMaterial* fbxMaterial,
                                    ResourceManager* resourceManager,
                                    Material* outMaterial);
        static FbxAMatrix GetGeometryTransform(FbxNode* node);
    };

} // namespace USE