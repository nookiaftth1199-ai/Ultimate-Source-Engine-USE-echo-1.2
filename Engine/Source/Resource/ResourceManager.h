// ============================================================
// Ultimate Source Engine - Resource Manager
// ============================================================
//
// Central resource management system. Loads, caches, and provides
// access to all game assets (textures, models, sounds, shaders, etc.).
// Uses the FileSystem to locate files and the render device to create GPU resources.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Model.h"
#include "Audio/SoundBuffer.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace USE {

    class FileSystem;
    class IRenderDevice;

    class ResourceManager {
    public:
        ResourceManager();
        ~ResourceManager();

        // Initialize with file system and render device
        bool Initialize(FileSystem* fileSystem, IRenderDevice* device);
        void Shutdown();

        // Load a texture from file (returns cached instance if already loaded)
        Texture* LoadTexture(const std::string& filename);

        // Load a model from file (returns cached instance)
        Model* LoadModel(const std::string& filename);

        // Load a sound buffer from file
        SoundBuffer* LoadSound(const std::string& filename);

        // Load a shader from source files
        Shader* LoadShader(const std::string& vertexFile, const std::string& fragmentFile,
                           const std::string& geometryFile = "");

        // Create a material (not loaded, just created; caller must set shader/textures)
        Material* CreateMaterial(const std::string& name);

        // Get a resource by name (if already loaded)
        Texture* GetTexture(const std::string& filename) const;
        Model*   GetModel(const std::string& filename) const;
        SoundBuffer* GetSound(const std::string& filename) const;
        Shader*  GetShader(const std::string& name) const; // name could be vertex+fragment combined
        Material* GetMaterial(const std::string& name) const;

        // Unload a specific resource (if needed)
        void UnloadTexture(const std::string& filename);
        void UnloadModel(const std::string& filename);
        void UnloadSound(const std::string& filename);
        void UnloadShader(const std::string& name);
        void UnloadMaterial(const std::string& name);

        // Unload all resources
        void UnloadAll();

        // Get statistics
        size_t GetTextureCount() const { return m_textures.size(); }
        size_t GetModelCount() const   { return m_models.size(); }
        size_t GetSoundCount() const   { return m_sounds.size(); }
        size_t GetShaderCount() const  { return m_shaders.size(); }
        size_t GetMaterialCount() const { return m_materials.size(); }

    private:
        FileSystem*     m_fileSystem;
        IRenderDevice*  m_device;

        std::unordered_map<std::string, std::unique_ptr<Texture>>     m_textures;
        std::unordered_map<std::string, std::unique_ptr<Model>>       m_models;
        std::unordered_map<std::string, std::unique_ptr<SoundBuffer>> m_sounds;
        std::unordered_map<std::string, std::unique_ptr<Shader>>      m_shaders;
        std::unordered_map<std::string, std::unique_ptr<Material>>    m_materials;

        // Helper to generate a unique key for shader from filenames
        std::string MakeShaderKey(const std::string& vertex, const std::string& fragment, const std::string& geometry) const;
    };

} // namespace USE