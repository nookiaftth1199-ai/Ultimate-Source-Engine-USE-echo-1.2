// ============================================================
// Ultimate Source Engine - Model
//============================================================
//
// Loads a 3D model from a file (FBX, OBJ, glTF, etc.) using Assimp.
// Creates a hierarchy of nodes, each containing meshes.
// Manages vertex data, indices, and materials.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Mesh.h"
#include "Material.h"
#include <string>
#include <vector>

namespace USE {

    // Forward declaration
    class IRenderDevice;
    class ResourceManager;

    // Model node (part of hierarchy)
    struct ModelNode {
        std::string         name;
        Matrix4             transform;          // local transform relative to parent
        std::vector<int>    childIndices;       // indices into model's node list
        std::vector<int>    meshIndices;        // indices into model's mesh list
    };

    class Model {
    public:
        Model();
        ~Model();

        // Load model from file using Assimp
        bool LoadFromFile(const char* filename,
                          IRenderDevice* device,
                          ResourceManager* resourceManager);

        // Destroy all loaded resources
        void Destroy();

        // Accessors
        const std::vector<Mesh*>&     GetMeshes() const     { return m_meshes; }
        const std::vector<Material*>& GetMaterials() const  { return m_materials; }
        const std::vector<ModelNode>& GetNodes() const      { return m_nodes; }
        int                           GetRootNodeIndex() const { return m_rootNodeIndex; }

        // Check if model is valid
        bool IsValid() const { return !m_meshes.empty(); }

    private:
        std::vector<Mesh*>      m_meshes;
        std::vector<Material*>  m_materials;        // materials used by meshes
        std::vector<ModelNode>  m_nodes;
        int                     m_rootNodeIndex;

        // Assimp loading helpers
        bool ProcessNode(void* aiNode, int parentIndex,
                         IRenderDevice* device,
                         ResourceManager* resourceManager);
        void ProcessMesh(void* aiMesh, void* aiMaterial,
                         int nodeIndex,
                         IRenderDevice* device,
                         ResourceManager* resourceManager);
        Material* ProcessMaterial(void* aiMaterial,
                                   ResourceManager* resourceManager);
    };

} // namespace USE