// ============================================================
// Ultimate Source Engine - glTF Loader Implementation
// ============================================================

#include "stdafx.h"
#include "GLTFLoader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Texture.h"
#include "Resources/ResourceManager.h"

// cgltf single header library
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <vector>
#include <unordered_map>

namespace USE {

    Model* GLTFLoader::LoadFromFile(const std::string& filename,
                                     IRenderDevice* device,
                                     ResourceManager* resourceManager)
    {
        if (!device || !resourceManager) {
            USE_LOG_ERROR("GLTFLoader: device or resourceManager is null");
            return nullptr;
        }

        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("GLTFLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("GLTFLoader: file not found: %s", filename.c_str());
            return nullptr;
        }

        // Load the glTF file using cgltf
        cgltf_options options = {};
        cgltf_data* data = nullptr;
        cgltf_result result = cgltf_parse_file(&options, resolved.c_str(), &data);
        if (result != cgltf_result_success) {
            USE_LOG_ERROR("GLTFLoader: Failed to parse glTF file: %s", filename.c_str());
            return nullptr;
        }

        // Load binary buffers (if any)
        result = cgltf_load_buffers(&options, data, resolved.c_str());
        if (result != cgltf_result_success) {
            USE_LOG_ERROR("GLTFLoader: Failed to load buffers for: %s", filename.c_str());
            cgltf_free(data);
            return nullptr;
        }

        // Create output model
        Model* model = new Model();

        // Process root node and its hierarchy (there is no single root node in glTF; we have a scene with nodes)
        // We'll create a root node for the model and attach all top‑level nodes as children.
        if (data->scene) {
            // Use the default scene
            for (size_t i = 0; i < data->scene->nodes_count; ++i) {
                ProcessNode(data->scene->nodes[i], -1, device, resourceManager, model);
            }
        } else if (data->nodes_count > 0) {
            // No default scene, process all root nodes (nodes without parent)
            // But cgltf doesn't give parent info; we can build hierarchy from node parents.
            // For simplicity, we'll process all nodes and build a flat list; later we can compute hierarchy.
            // Better: we can detect root nodes by checking if they have no parent.
            // We'll create a temporary map of parent pointers.
            std::unordered_map<cgltf_node*, cgltf_node*> parentMap;
            for (size_t i = 0; i < data->nodes_count; ++i) {
                for (size_t j = 0; j < data->nodes[i].children_count; ++j) {
                    parentMap[data->nodes[i].children[j]] = &data->nodes[i];
                }
            }
            for (size_t i = 0; i < data->nodes_count; ++i) {
                if (parentMap.find(&data->nodes[i]) == parentMap.end()) {
                    // Root node
                    ProcessNode(&data->nodes[i], -1, device, resourceManager, model);
                }
            }
        } else {
            USE_LOG_WARN("GLTFLoader: No nodes found in file: %s", filename.c_str());
        }

        cgltf_free(data);

        USE_LOG_INFO("GLTFLoader: Loaded %s: %zu meshes, %zu materials",
                     filename.c_str(), model->GetMeshes().size(),
                     model->GetMaterials().size());
        return model;
    }

    bool GLTFLoader::ProcessNode(void* gltfNodeVoid, int parentIndex,
                                 IRenderDevice* device,
                                 ResourceManager* resourceManager,
                                 Model* outModel)
    {
        cgltf_node* node = static_cast<cgltf_node*>(gltfNodeVoid);

        ModelNode modelNode;
        modelNode.name = node->name ? node->name : "UnnamedNode";

        // Convert glTF matrix to our Matrix4 (column‑major)
        if (node->has_matrix) {
            // glTF matrices are column‑major already (same as us)
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    modelNode.transform.m[i][j] = node->matrix[i * 4 + j];
        } else {
            // Compute from TRS
            Vector3 translation(node->translation[0], node->translation[1], node->translation[2]);
            Vector3 scale(node->scale[0], node->scale[1], node->scale[2]);
            Quaternion rotation(node->rotation[0], node->rotation[1],
                                node->rotation[2], node->rotation[3]);

            modelNode.transform = Transform(translation, rotation, scale).ToMatrix();
        }

        int nodeIndex = static_cast<int>(outModel->GetNodes().size());
        outModel->m_nodes.push_back(modelNode);

        // Process mesh attached to this node
        if (node->mesh) {
            ProcessMesh(node->mesh, 0, nodeIndex, device, resourceManager, outModel);
        }

        // Process children
        for (size_t i = 0; i < node->children_count; ++i) {
            int childIndex = ProcessNode(node->children[i], nodeIndex,
                                         device, resourceManager, outModel);
            outModel->m_nodes[nodeIndex].childIndices.push_back(childIndex);
        }

        return true;
    }

    void GLTFLoader::ProcessMesh(void* gltfMeshVoid, int meshIndex,
                                 int nodeIndex,
                                 IRenderDevice* device,
                                 ResourceManager* resourceManager,
                                 Model* outModel)
    {
        cgltf_mesh* mesh = static_cast<cgltf_mesh*>(gltfMeshVoid);

        // Each mesh can have multiple primitives (each is a separate drawable)
        for (size_t i = 0; i < mesh->primitives_count; ++i) {
            cgltf_primitive* prim = &mesh->primitives[i];

            // We'll collect vertex and index data
            std::vector<float> vertices;
            std::vector<uint32_t> indices;

            // Determine vertex stride and layout
            // We'll support positions, normals, texcoords (first set), tangents.
            // For simplicity, we'll use a fixed interleaved format: position (3), normal (3), texcoord (2), tangent (4?) – actually tangent is 4.
            // But glTF tangent is 3 (or 4 with handedness). We'll use 3 and compute bitangent in shader, or store 4.
            // We'll store as 3 for simplicity, and we can generate bitangent in shader.

            // We'll build a list of attributes we need.
            struct Vertex {
                float pos[3];
                float normal[3];
                float texcoord[2];
                float tangent[3];
                // could add color, etc.
            };

            // First, count vertices
            cgltf_accessor* posAccessor = nullptr;
            cgltf_accessor* normalAccessor = nullptr;
            cgltf_accessor* texcoordAccessor = nullptr;
            cgltf_accessor* tangentAccessor = nullptr;

            for (size_t a = 0; a < prim->attributes_count; ++a) {
                cgltf_attribute* attr = &prim->attributes[a];
                if (attr->type == cgltf_attribute_type_position) posAccessor = attr->data;
                else if (attr->type == cgltf_attribute_type_normal) normalAccessor = attr->data;
                else if (attr->type == cgltf_attribute_type_texcoord && attr->index == 0) texcoordAccessor = attr->data;
                else if (attr->type == cgltf_attribute_type_tangent) tangentAccessor = attr->data;
            }

            if (!posAccessor) {
                USE_LOG_ERROR("GLTFLoader: Primitive missing position attribute");
                return;
            }

            size_t vertexCount = posAccessor->count;

            // Read index data
            if (prim->indices) {
                indices.resize(prim->indices->count);
                cgltf_accessor_unpack_indices(prim->indices, &indices[0], indices.size() * sizeof(uint32_t));
            } else {
                // Non-indexed geometry: generate sequential indices
                indices.resize(vertexCount);
                for (size_t i = 0; i < vertexCount; ++i) indices[i] = (uint32_t)i;
            }

            // Read vertex data
            std::vector<Vertex> verts(vertexCount);

            // Read positions
            cgltf_accessor_unpack_floats(posAccessor, &verts[0].pos[0], vertexCount * 3);

            // Read normals (if present)
            if (normalAccessor) {
                cgltf_accessor_unpack_floats(normalAccessor, &verts[0].normal[0], vertexCount * 3);
            } else {
                // If no normals, we could generate them, but for now set to zero
                for (size_t i = 0; i < vertexCount; ++i) {
                    verts[i].normal[0] = verts[i].normal[1] = verts[i].normal[2] = 0;
                }
            }

            // Read texcoords (if present)
            if (texcoordAccessor) {
                cgltf_accessor_unpack_floats(texcoordAccessor, &verts[0].texcoord[0], vertexCount * 2);
            } else {
                for (size_t i = 0; i < vertexCount; ++i) {
                    verts[i].texcoord[0] = verts[i].texcoord[1] = 0;
                }
            }

            // Read tangents (if present)
            if (tangentAccessor) {
                cgltf_accessor_unpack_floats(tangentAccessor, &verts[0].tangent[0], vertexCount * 3);
            } else {
                for (size_t i = 0; i < vertexCount; ++i) {
                    verts[i].tangent[0] = verts[i].tangent[1] = verts[i].tangent[2] = 0;
                }
            }

            // Create mesh object
            Mesh* meshObj = new Mesh();
            if (!meshObj->Create(device,
                                 verts.data(),
                                 static_cast<uint32_t>(vertexCount),
                                 sizeof(Vertex),
                                 indices.data(),
                                 static_cast<uint32_t>(indices.size()))) {
                USE_LOG_ERROR("GLTFLoader: failed to create Mesh");
                delete meshObj;
                return;
            }

            // Add mesh to model
            int meshIndex = static_cast<int>(outModel->m_meshes.size());
            outModel->m_meshes.push_back(meshObj);

            // Associate mesh with node
            outModel->m_nodes[nodeIndex].meshIndices.push_back(meshIndex);

            // Process material for this primitive
            if (prim->material) {
                Material* mat = ProcessMaterial(prim->material, resourceManager);
                outModel->m_materials.push_back(mat);
                // We need to map the primitive to its material; we can store the material index in the mesh.
                // For now, we'll push material and assume meshes are in order.
                // A better approach: store a material index per primitive, but we are only creating one mesh per primitive.
                // We can associate material index with mesh by storing a separate array.
                // For simplicity, we'll create a material per mesh and push to m_materials in same order as m_meshes.
                // This works if each primitive gets its own mesh.
                // So after mesh is added, we also add its material to m_materials.
                outModel->m_materials.push_back(mat);
            } else {
                // Default material (if no material assigned)
                Material* defaultMat = resourceManager->CreateMaterial("glTF_default");
                // Set some default properties if needed
                outModel->m_materials.push_back(defaultMat);
            }
        }
    }

    Material* GLTFLoader::ProcessMaterial(void* gltfMaterialVoid,
                                          ResourceManager* resourceManager)
    {
        cgltf_material* mat = static_cast<cgltf_material*>(gltfMaterialVoid);

        // Create a unique name
        std::string name = mat->name ? mat->name : "glTF_material";
        Material* material = resourceManager->CreateMaterial(name);

        // Load base color texture (PBR metallic‑roughness)
        if (mat->has_pbr_metallic_roughness) {
            cgltf_texture_view* baseColor = &mat->pbr_metallic_roughness.base_color_texture;
            if (baseColor->texture && baseColor->texture->image) {
                // The image may have a URI; we need to load it via resource manager
                // The URI might be a relative path to the glTF file. We'll need to resolve.
                // For now, we just pass the URI string; ResourceManager will resolve using FileSystem.
                Texture* tex = resourceManager->LoadTexture(baseColor->texture->image->uri);
                if (tex) material->SetTexture(0, tex); // slot 0 for diffuse/baseColor
            }
            // Base color factor
            material->SetUniform("material.baseColorFactor",
                                 Color(mat->pbr_metallic_roughness.base_color_factor[0],
                                       mat->pbr_metallic_roughness.base_color_factor[1],
                                       mat->pbr_metallic_roughness.base_color_factor[2],
                                       mat->pbr_metallic_roughness.base_color_factor[3]));
        }

        // Load normal texture
        if (mat->normal_texture.texture && mat->normal_texture.texture->image) {
            Texture* tex = resourceManager->LoadTexture(mat->normal_texture.texture->image->uri);
            if (tex) material->SetTexture(1, tex); // slot 1 for normal
        }

        // Load metallic‑roughness texture
        if (mat->has_pbr_metallic_roughness) {
            cgltf_texture_view* mrTex = &mat->pbr_metallic_roughness.metallic_roughness_texture;
            if (mrTex->texture && mrTex->texture->image) {
                Texture* tex = resourceManager->LoadTexture(mrTex->texture->image->uri);
                if (tex) material->SetTexture(2, tex); // slot 2 for metallic/roughness (combined)
            }
            // Metallic and roughness factors
            material->SetUniform("material.metallicFactor",
                                 mat->pbr_metallic_roughness.metallic_factor);
            material->SetUniform("material.roughnessFactor",
                                 mat->pbr_metallic_roughness.roughness_factor);
        }

        // Load emissive texture
        if (mat->emissive_texture.texture && mat->emissive_texture.texture->image) {
            Texture* tex = resourceManager->LoadTexture(mat->emissive_texture.texture->image->uri);
            if (tex) material->SetTexture(3, tex); // slot 3 for emissive
        }
        material->SetUniform("material.emissiveFactor",
                             Color(mat->emissive_factor[0],
                                   mat->emissive_factor[1],
                                   mat->emissive_factor[2], 1.0f));

        // Alpha mode
        if (mat->alpha_mode == cgltf_alpha_mode_opaque) {
            material->SetBlendMode(BlendMode::Opaque);
        } else if (mat->alpha_mode == cgltf_alpha_mode_mask) {
            material->SetBlendMode(BlendMode::Opaque); // for mask, we need alpha test; not implemented yet
            material->SetUniform("material.alphaCutoff", mat->alpha_cutoff);
        } else if (mat->alpha_mode == cgltf_alpha_mode_blend) {
            material->SetBlendMode(BlendMode::AlphaBlend);
        }

        return material;
    }

} // namespace USE