// ============================================================
// Ultimate Source Engine - Model Loader Implementation
// ============================================================

#include "stdafx.h"
#include "ModelLoader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"
#include "Resources/ResourceManager.h"

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace USE {

    Model* ModelLoader::LoadFromFile(const std::string& filename,
                                      IRenderDevice* device,
                                      ResourceManager* resourceManager)
    {
        if (!device || !resourceManager) {
            USE_LOG_ERROR("ModelLoader: device or resourceManager is null");
            return nullptr;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ModelLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ModelLoader: file not found: %s", filename.c_str());
            return nullptr;
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
            USE_LOG_ERROR("ModelLoader: Assimp error: %s", importer.GetErrorString());
            return nullptr;
        }

        // Create output model
        Model* model = new Model();

        // Process root node recursively
        int rootNodeIndex = ProcessNode(scene->mRootNode, -1, device, resourceManager, model);
        if (rootNodeIndex == -1) {
            USE_LOG_ERROR("ModelLoader: Failed to process root node");
            delete model;
            return nullptr;
        }

        // Store root node index (model can have a method to set it)
        // For now, we rely on the model's internal nodes list; the root is the first node.

        USE_LOG_INFO("ModelLoader: Loaded %s: %zu meshes, %zu materials, %zu nodes",
                     filename.c_str(), model->GetMeshes().size(),
                     model->GetMaterials().size(), model->GetNodes().size());
        return model;
    }

    bool ModelLoader::ProcessNode(void* aiNodeVoid, int parentIndex,
                                  IRenderDevice* device,
                                  ResourceManager* resourceManager,
                                  Model* outModel)
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
        ).Transposed(); // Assimp matrices are row‑major

        // Add this node to the model
        int nodeIndex = static_cast<int>(outModel->GetNodes().size());
        outModel->m_nodes.push_back(modelNode);

        // Process meshes attached to this node
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            unsigned int meshIndex = node->mMeshes[i];
            aiMesh* aiMesh = scene->mMeshes[meshIndex];
            aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];
            ProcessMesh(aiMesh, aiMaterial, nodeIndex, device, resourceManager, outModel);
        }

        // Process children
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            int childIndex = ProcessNode(node->mChildren[i], nodeIndex,
                                         device, resourceManager, outModel);
            outModel->m_nodes[nodeIndex].childIndices.push_back(childIndex);
        }

        return nodeIndex;
    }

    void ModelLoader::ProcessMesh(void* aiMeshVoid, void* aiMaterialVoid,
                                  int nodeIndex,
                                  IRenderDevice* device,
                                  ResourceManager* resourceManager,
                                  Model* outModel)
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
                v.normal[0] = v.normal[1] = v.normal[2] = 0;
            }

            if (mesh->HasTextureCoords(0)) {
                v.texCoord[0] = mesh->mTextureCoords[0][i].x;
                v.texCoord[1] = mesh->mTextureCoords[0][i].y;
            } else {
                v.texCoord[0] = v.texCoord[1] = 0;
            }

            if (mesh->HasTangentsAndBitangents()) {
                v.tangent[0] = mesh->mTangents[i].x;
                v.tangent[1] = mesh->mTangents[i].y;
                v.tangent[2] = mesh->mTangents[i].z;
                v.bitangent[0] = mesh->mBitangents[i].x;
                v.bitangent[1] = mesh->mBitangents[i].y;
                v.bitangent[2] = mesh->mBitangents[i].z;
            } else {
                v.tangent[0] = v.tangent[1] = v.tangent[2] = 0;
                v.bitangent[0] = v.bitangent[1] = v.bitangent[2] = 0;
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
                             static_cast<uint32_t>(vertices.size()),
                             sizeof(Vertex),
                             indices.data(),
                             static_cast<uint32_t>(indices.size()))) {
            USE_LOG_ERROR("ModelLoader::ProcessMesh: failed to create Mesh");
            delete meshObj;
            return;
        }

        // Get or create Material
        Material* mat = ProcessMaterial(aiMat, resourceManager);

        // Store mesh and material
        outModel->m_meshes.push_back(meshObj);
        outModel->m_materials.push_back(mat);

        // Associate mesh with node
        int meshIndex = static_cast<int>(outModel->m_meshes.size()) - 1;
        outModel->m_nodes[nodeIndex].meshIndices.push_back(meshIndex);
    }

    Material* ModelLoader::ProcessMaterial(void* aiMaterialVoid,
                                           ResourceManager* resourceManager)
    {
        aiMaterial* aiMat = static_cast<aiMaterial*>(aiMaterialVoid);

        // Generate a unique name (use pointer address as temporary)
        char name[64];
        snprintf(name, sizeof(name), "material_%p", aiMat);
        Material* mat = resourceManager->CreateMaterial(name);

        // Load diffuse texture
        aiString texPath;
        if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(0, tex); // slot 0 for diffuse
            }
        }

        // Load normal map
        if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(1, tex); // slot 1 for normal
            }
        }

        // Load specular texture
        if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS) {
            Texture* tex = resourceManager->LoadTexture(texPath.C_Str());
            if (tex) {
                mat->SetTexture(2, tex); // slot 2 for specular
            }
        }

        // Set material properties
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