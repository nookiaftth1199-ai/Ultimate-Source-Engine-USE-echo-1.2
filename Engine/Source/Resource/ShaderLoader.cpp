// ============================================================
// Ultimate Source Engine - Shader Loader Implementation
// ============================================================

#include "stdafx.h"
#include "ShaderLoader.h"
#include "Renderer/Shader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    Shader* ShaderLoader::LoadFromFile(const std::string& vertexPath,
                                       const std::string& fragmentPath,
                                       const std::string& geometryPath)
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ShaderLoader: FileSystem not available");
            return nullptr;
        }

        // Resolve paths (just get the absolute paths for reading)
        std::string resolvedV = fs->ResolvePath(vertexPath);
        if (resolvedV.empty()) {
            USE_LOG_ERROR("ShaderLoader: Vertex shader file not found: %s", vertexPath.c_str());
            return nullptr;
        }
        std::string resolvedF = fs->ResolvePath(fragmentPath);
        if (resolvedF.empty()) {
            USE_LOG_ERROR("ShaderLoader: Fragment shader file not found: %s", fragmentPath.c_str());
            return nullptr;
        }
        std::string resolvedG;
        if (!geometryPath.empty()) {
            resolvedG = fs->ResolvePath(geometryPath);
            if (resolvedG.empty()) {
                USE_LOG_WARN("ShaderLoader: Geometry shader file not found: %s", geometryPath.c_str());
                // Continue without geometry shader
            }
        }

        // Read vertex source
        auto vFile = fs->OpenFile(resolvedV, FILE_READ | FILE_TEXT);
        if (!vFile) {
            USE_LOG_ERROR("ShaderLoader: Could not open vertex shader file: %s", resolvedV.c_str());
            return nullptr;
        }
        std::string vSource;
        vFile->ReadAll(vSource);
        vFile->Close();

        // Read fragment source
        auto fFile = fs->OpenFile(resolvedF, FILE_READ | FILE_TEXT);
        if (!fFile) {
            USE_LOG_ERROR("ShaderLoader: Could not open fragment shader file: %s", resolvedF.c_str());
            return nullptr;
        }
        std::string fSource;
        fFile->ReadAll(fSource);
        fFile->Close();

        // Read geometry source if provided
        std::string gSource;
        if (!geometryPath.empty() && !resolvedG.empty()) {
            auto gFile = fs->OpenFile(resolvedG, FILE_READ | FILE_TEXT);
            if (gFile) {
                gFile->ReadAll(gSource);
                gFile->Close();
            }
        }

        // Use LoadFromSource to do the actual compilation
        return LoadFromSource(vSource, fSource, gSource);
    }

    Shader* ShaderLoader::LoadFromSource(const std::string& vertexSource,
                                         const std::string& fragmentSource,
                                         const std::string& geometrySource)
    {
        Shader* shader = Shader::Create();
        if (!shader) {
            USE_LOG_ERROR("ShaderLoader: Failed to create shader object (backend unavailable?)");
            return nullptr;
        }

        bool success;
        if (geometrySource.empty()) {
            success = shader->LoadFromSource(vertexSource.c_str(), fragmentSource.c_str());
        } else {
            success = shader->LoadFromSource(vertexSource.c_str(), fragmentSource.c_str(), geometrySource.c_str());
        }

        if (!success) {
            USE_LOG_ERROR("ShaderLoader: Failed to compile/link shader");
            delete shader;
            return nullptr;
        }

        return shader;
    }

} // namespace USE