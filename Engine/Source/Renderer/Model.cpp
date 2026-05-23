// ============================================================
// Ultimate Source Engine - Model Implementation
// ============================================================

#include "stdafx.h"
#include "Model.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Resources/ResourceManager.h"

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace USE {

    Model::Model()
        : m_rootNodeIndex(-1)
    {
    }

    Model::~Model()
    {
        Destroy();
    }

    bool Model::LoadFromFile(const char* filename,
                              IRenderDevice* device,
                              ResourceManager* resourceManager)
    {
        if (!device) {
            USE_LOG_ERROR("Model::LoadFromFile: device is null");
            return false;
        }
        if (!resourceManager) {
            USE_LOG_ERROR("Model::LoadFromFile: resourceManager is null");
            return false;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("Model::LoadFromFile: FileSystem not available");
            return false;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("Model::LoadFromFile: file not found: %s", filename);
            return false;
        }

        // Create Assimp importer
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(resolved,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_GenUVCoords |
            aiProcess_OptimizeMeshes |
            aiProcess_ValidateDataStructure);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            USE_LOG_ERROR("Model::LoadFromFile: Assimp error: %s", importer.GetErrorString());
            return false;
        }

        // Clear previous data
        Destroy();

        // Process root node recursively
        m_rootNodeIndex = ProcessNode(scene->mRootNode, -1, device, resourceManager);

        if (m_rootNodeIndex == -1) {
            USE_LOG_ERROR("Model::LoadFromFile: Failed to process root node");
            return false;
        }

        USE_LOG_INFO("Model loaded: %s (%zu meshes, %zu materials, %zu nodes)",
                     filename, m_meshes.size(), m_materials.size(), m_nodes.size());
        return true;
    }

    void Model::Destroy()
    {
        for (Mesh* mesh : m_meshes) {
            delete mesh;    // Mesh destructor should release GPU buffers (requires device)
            // Note: Mesh destructor currently does not release GPU buffers automatically.
            // In a full implementation, Mesh should be reference-counted or have a Destroy method that takes device.
        }
        m_meshes.clear();

        for (Material* mat : m_materials) {
            delete mat;     // Material destructor does not release textures (they are managed by resource manager)
        }
        m_materials.clear();

        m_nodes.clear();
        m_rootNodeIndex = -1;
    }

    int Model::ProcessNode(void* aiNodeVoid, int parentIndex,
                           IRenderDevice* device,
                           ResourceManager* resourceManager)
    {
        aiNode* node = static_cast<aiNode*>(aiNodeVoid);

        ModelNode modelNode;
        modelNode.name = node->mName.C_Str();

        // Convert aiMatrix4x4 to our Matrix4 (column‑major)
        aiMatrix4x4& m = node->mTransformation;
        modelNode.transform = Matrix4(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        ).Transposed(); // Assimp matrices are row‑major; we need column‑major.

        // Process meshes attached to this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            unsigned int meshIndex = node->mMeshes[i];
            aiMesh* aiMesh = m_scene->mMeshes[meshIndex];
            aiMaterial* aiMaterial = m_scene->mMaterials[aiMesh->mMaterialIndex];

            // Process the mesh and add its index to node
            int meshListIndex = (int)m_meshes.size();
            ProcessMesh(aiMesh, aiMaterial, meshListIndex, device, resourceManager);
            modelNode.meshIndices.push_back(meshListIndex);
        }

        // Add this node to the list
        int nodeIndex = (int)m_nodes.size();
        m_nodes.push_back(modelNode);

        // Process children
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            int childIndex = ProcessNode(node->mChildren[i], nodeIndex, device, resourceManager);
            m_nodes[nodeIndex].childIndices.push_back(childIndex);
        }

        return nodeIndex;
    }

    void Model::ProcessMesh(void* aiMeshVoid, void* aiMaterialVoid,
                            int meshListIndex,
                            IRenderDevice* device,
                            ResourceManager* resourceManager)
    {
        aiMesh* mesh = static_cast<aiMesh*>(aiMeshVoid);
        aiMaterial* aiMat = static_cast<aiMaterial*>(aiMaterialVoid);

        // Build vertex buffer (interleaved)
        struct Vertex {
            float position[3];
            float normal[3];
            float texCoord[2];
            float tangent[3];
            float bitangent[3];
        };

        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            Vertex v;
            v.position[0] = mesh->mVertices[i].x;
            v.position[1] = mesh->mVertices[i].y;
            v.position[2] = mesh->mVertices[i].z;

            if (mesh->HasNormals()) {
                v.normal[0] = mesh->mNormals[i].x;
                v.normal[1] = mesh->mNormals[i].y;
                v.normal[2] = mesh->mNormals[i].z;
            } else {
                v.normal[0] = 0; v.normal[1] = 0; v.normal[2] = 0;
            }

            if (mesh->HasTextureCoords(0)) {
                v.texCoord[0] = mesh->mTextureCoords[0][i].x;
                v.texCoord[1] = mesh->mTextureCoords[0][i].y;
            } else {
                v.texCoord[0] = 0; v.texCoord[1] = 0;
            }

            if (mesh->HasTangentsAndBitangents()) {
                v.tangent[0] = mesh->mTangents[i].x;
                v.tangent[1] = mesh->mTangents[i].y;
                v.tangent[2] = mesh->mTangents[i].z;
                v.bitangent[0] = mesh->mBitangents[i].x;
                v.bitangent[1] = mesh->mBitangents[i].y;
                v.bitangent[2] = mesh->mBitangents[i].z;
            } else {
                v.tangent[0] = 0; v.tangent[1] = 0; v.tangent[2] = 0;
                v.bitangent[0] = 0; v.bitangent[1] = 0; v.bitangent[2] = 0;
            }

            vertices.push_back(v);
        }

        // Build index buffer
        std::vector<uint32_t> indices;
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Create Mesh
        Mesh* meshObj = new Mesh();
        if (!meshObj->Create(device,
                             vertices.data(),
                             (uint32_t)vertices.size(),
                             sizeof(Vertex),
                             indices.data(),
                             (uint32_t)indices.size())) {
            USE_LOG_ERROR("Model::ProcessMesh: failed to create Mesh");
            delete meshObj;
            return;
        }

        // Get or create Material
        Material* mat = ProcessMaterial(aiMat, resourceManager);

        // Store mesh and material (mesh stores material index? For now, we just push mesh and later user can assign materials.)
        // In this simple version, we don't store material per mesh; we just push the material separately.
        // The user will need to assign materials to meshes based on some logic.
        // We'll store the material index in the node or mesh? For simplicity, we'll just push the material to m_materials
        // and the mesh will be used without material association. Alternatively, we could store a pair (mesh, materialIndex).
        // For now, we just push mesh and material separately; user can later bind materials via material list.

        m_meshes.push_back(meshObj);
        // If this material is not already in m_materials, add it
        // We need to compare materials; for now, we just push each new material (duplicates may occur)
        // A better approach would be to have a material cache.
        m_materials.push_back(mat); // potentially duplicates; we ignore for simplicity.
    }

    Material* Model::ProcessMaterial(void* aiMaterialVoid,
                                     ResourceManager* resourceManager)
    {
        aiMaterial* aiMat = static_cast<aiMaterial*>(aiMaterialVoid);

        Material* mat = new Material("AssimpMaterial"); // name could be improved

        // Load diffuse texture
        aiString texPath;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(0, tex); // slot 0 for diffuse
            }
        }

        // Load other textures (normal, specular, etc.)
        if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(1, tex); // slot 1 for normal
            }
        }
        if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(2, tex); // slot 2 for specular
            }
        }

        // Set material properties (color, shininess, etc.)
        aiColor4D color;
        if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            mat->SetUniform("material.diffuse", Color(color.r, color.g, color.b, color.a));
        }
        if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            mat->SetUniform("material.specular", Color(color.r, color.g, color.b, color.a));
        }
        float shininess;
        if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            mat->SetUniform("material.shininess", shininess);
        }

        return mat;
    }

} // namespace USE