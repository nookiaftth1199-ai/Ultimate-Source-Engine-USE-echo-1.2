// ============================================================
// Ultimate Source Engine - Material Loader Implementation
// ============================================================

#include "stdafx.h"
#include "MaterialLoader.h"
#include "Resources/ResourceManager.h"
#include "Core/Logger.h"
#include "Core/FileSystem.h"

// JSON library (nlohmann/json)
#include "json.hpp"
using json = nlohmann::json;

namespace USE {

    Material* MaterialLoader::LoadFromFile(const std::string& filename,
                                           ResourceManager* resourceManager)
    {
        if (!resourceManager) {
            USE_LOG_ERROR("MaterialLoader: ResourceManager is null");
            return nullptr;
        }

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("MaterialLoader: FileSystem not available");
            return nullptr;
        }

        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("MaterialLoader: file not found: %s", filename.c_str());
            return nullptr;
        }

        // Read file contents
        auto file = fs->OpenFile(resolved, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_ERROR("MaterialLoader: Could not open file: %s", resolved.c_str());
            return nullptr;
        }

        std::string content;
        file->ReadAll(content);
        file->Close();

        // Parse JSON
        json j;
        try {
            j = json::parse(content);
        } catch (const std::exception& e) {
            USE_LOG_ERROR("MaterialLoader: JSON parse error in %s: %s",
                          filename.c_str(), e.what());
            return nullptr;
        }

        // Get material name
        std::string name = j.value("name", "UnnamedMaterial");
        Material* material = new Material(name.c_str());

        // Load shader
        if (j.contains("shader")) {
            const auto& shaderJson = j["shader"];
            std::string vertex = shaderJson.value("vertex", "");
            std::string fragment = shaderJson.value("fragment", "");
            std::string geometry = shaderJson.value("geometry", "");
            if (!vertex.empty() && !fragment.empty()) {
                Shader* shader = resourceManager->LoadShader(vertex, fragment, geometry);
                if (shader) {
                    material->SetShader(shader);
                } else {
                    USE_LOG_WARN("MaterialLoader: Failed to load shader for material %s",
                                 name.c_str());
                }
            }
        }

        // Load textures
        if (j.contains("textures")) {
            const auto& texs = j["textures"];
            for (const auto& tex : texs) {
                int slot = tex.value("slot", 0);
                std::string path = tex.value("path", "");
                if (!path.empty()) {
                    Texture* texture = resourceManager->LoadTexture(path);
                    if (texture) {
                        material->SetTexture(slot, texture);
                    } else {
                        USE_LOG_WARN("MaterialLoader: Failed to load texture %s", path.c_str());
                    }
                }
            }
        }

        // Parse render states
        if (j.contains("renderStates")) {
            const auto& rs = j["renderStates"];

            // Blend mode
            std::string blendMode = rs.value("blendMode", "opaque");
            if (blendMode == "alpha") material->SetBlendMode(BlendMode::AlphaBlend);
            else if (blendMode == "additive") material->SetBlendMode(BlendMode::Additive);
            else if (blendMode == "multiply") material->SetBlendMode(BlendMode::Multiply);
            else material->SetBlendMode(BlendMode::Opaque);

            // Depth test / write
            material->SetDepthTest(rs.value("depthTest", true));
            material->SetDepthWrite(rs.value("depthWrite", true));

            // Cull mode
            std::string cull = rs.value("cullMode", "back");
            if (cull == "none") material->SetCullMode(CullMode::None);
            else if (cull == "front") material->SetCullMode(CullMode::Front);
            else material->SetCullMode(CullMode::Back);

            // Fill mode
            std::string fill = rs.value("fillMode", "solid");
            if (fill == "wireframe") material->SetFillMode(FillMode::Wireframe);
            else material->SetFillMode(FillMode::Solid);
        }

        // Parse uniforms
        if (j.contains("uniforms")) {
            ParseUniforms(material, j["uniforms"]);
        }

        USE_LOG_INFO("MaterialLoader: Loaded material '%s' from %s",
                     name.c_str(), filename.c_str());
        return material;
    }

    bool MaterialLoader::ParseUniforms(Material* material, const json& j)
    {
        // Iterate over each uniform entry
        for (auto it = j.begin(); it != j.end(); ++it) {
            const std::string& name = it.key();
            const json& value = it.value();

            if (value.is_number_integer()) {
                material->SetUniform(name.c_str(), value.get<int>());
            } else if (value.is_number_float()) {
                material->SetUniform(name.c_str(), value.get<float>());
            } else if (value.is_boolean()) {
                material->SetUniform(name.c_str(), value.get<bool>());
            } else if (value.is_array()) {
                // Determine array type (vector2,3,4, color, or matrix)
                if (value.size() == 2) {
                    Vector2 v(value[0].get<float>(), value[1].get<float>());
                    material->SetUniform(name.c_str(), v);
                } else if (value.size() == 3) {
                    Vector3 v(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
                    material->SetUniform(name.c_str(), v);
                } else if (value.size() == 4) {
                    // Could be Vector4 or Color. We'll treat as Color for simplicity.
                    Color c(value[0].get<float>(), value[1].get<float>(),
                            value[2].get<float>(), value[3].get<float>());
                    material->SetUniform(name.c_str(), c);
                } else if (value.size() == 16) {
                    // Matrix4 (column‑major order expected)
                    Matrix4 m;
                    for (int i = 0; i < 4; ++i)
                        for (int j = 0; j < 4; ++j)
                            m.m[i][j] = value[i * 4 + j].get<float>();
                    material->SetUniform(name.c_str(), m, false);
                } else {
                    USE_LOG_WARN("MaterialLoader: Unsupported uniform array size for '%s'", name.c_str());
                }
            } else {
                USE_LOG_WARN("MaterialLoader: Unsupported uniform type for '%s'", name.c_str());
            }
        }
        return true;
    }

} // namespace USE