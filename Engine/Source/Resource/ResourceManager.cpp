// ============================================================
// Ultimate Source Engine - Resource Manager Implementation
// ============================================================

#include "stdafx.h"
#include "ResourceManager.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    ResourceManager::ResourceManager()
        : m_fileSystem(nullptr)
        , m_device(nullptr)
    {
    }

    ResourceManager::~ResourceManager()
    {
        Shutdown();
    }

    bool ResourceManager::Initialize(FileSystem* fileSystem, IRenderDevice* device)
    {
        m_fileSystem = fileSystem;
        m_device = device;
        return true;
    }

    void ResourceManager::Shutdown()
    {
        UnloadAll();
    }

    Texture* ResourceManager::LoadTexture(const std::string& filename)
    {
        // Check if already loaded
        auto it = m_textures.find(filename);
        if (it != m_textures.end()) {
            return it->second.get();
        }

        // Resolve path via file system
        std::string resolved = m_fileSystem->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ResourceManager: Texture file not found: %s", filename.c_str());
            return nullptr;
        }

        // Create texture using factory (backend‑specific)
        Texture* tex = Texture::Create();
        if (!tex || !tex->LoadFromFile(resolved.c_str())) {
            USE_LOG_ERROR("ResourceManager: Failed to load texture: %s", filename.c_str());
            delete tex;
            return nullptr;
        }

        m_textures[filename] = std::unique_ptr<Texture>(tex);
        USE_LOG_INFO("ResourceManager: Loaded texture: %s", filename.c_str());
        return tex;
    }

    Model* ResourceManager::LoadModel(const std::string& filename)
    {
        auto it = m_models.find(filename);
        if (it != m_models.end()) return it->second.get();

        std::string resolved = m_fileSystem->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ResourceManager: Model file not found: %s", filename.c_str());
            return nullptr;
        }

        Model* model = new Model();
        if (!model->LoadFromFile(resolved.c_str(), m_device, this)) {
            USE_LOG_ERROR("ResourceManager: Failed to load model: %s", filename.c_str());
            delete model;
            return nullptr;
        }

        m_models[filename] = std::unique_ptr<Model>(model);
        USE_LOG_INFO("ResourceManager: Loaded model: %s", filename.c_str());
        return model;
    }

    SoundBuffer* ResourceManager::LoadSound(const std::string& filename)
    {
        auto it = m_sounds.find(filename);
        if (it != m_sounds.end()) return it->second.get();

        std::string resolved = m_fileSystem->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ResourceManager: Sound file not found: %s", filename.c_str());
            return nullptr;
        }

        SoundBuffer* sound = new SoundBuffer();
        if (!sound->LoadFromFile(resolved.c_str())) {
            USE_LOG_ERROR("ResourceManager: Failed to load sound: %s", filename.c_str());
            delete sound;
            return nullptr;
        }

        m_sounds[filename] = std::unique_ptr<SoundBuffer>(sound);
        USE_LOG_INFO("ResourceManager: Loaded sound: %s", filename.c_str());
        return sound;
    }

    Shader* ResourceManager::LoadShader(const std::string& vertexFile, const std::string& fragmentFile,
                                        const std::string& geometryFile)
    {
        std::string key = MakeShaderKey(vertexFile, fragmentFile, geometryFile);
        auto it = m_shaders.find(key);
        if (it != m_shaders.end()) return it->second.get();

        std::string resolvedV = m_fileSystem->ResolvePath(vertexFile);
        std::string resolvedF = m_fileSystem->ResolvePath(fragmentFile);
        if (resolvedV.empty() || resolvedF.empty()) {
            USE_LOG_ERROR("ResourceManager: Shader file(s) not found: %s / %s",
                          vertexFile.c_str(), fragmentFile.c_str());
            return nullptr;
        }

        Shader* shader = Shader::Create();
        if (geometryFile.empty()) {
            if (!shader->LoadFromFile(resolvedV.c_str(), resolvedF.c_str())) {
                USE_LOG_ERROR("ResourceManager: Failed to load shader: %s / %s",
                              vertexFile.c_str(), fragmentFile.c_str());
                delete shader;
                return nullptr;
            }
        } else {
            std::string resolvedG = m_fileSystem->ResolvePath(geometryFile);
            if (resolvedG.empty()) {
                USE_LOG_ERROR("ResourceManager: Geometry shader file not found: %s", geometryFile.c_str());
                delete shader;
                return nullptr;
            }
            if (!shader->LoadFromFile(resolvedV.c_str(), resolvedF.c_str(), resolvedG.c_str())) {
                USE_LOG_ERROR("ResourceManager: Failed to load shader (with geometry): %s / %s / %s",
                              vertexFile.c_str(), fragmentFile.c_str(), geometryFile.c_str());
                delete shader;
                return nullptr;
            }
        }

        m_shaders[key] = std::unique_ptr<Shader>(shader);
        USE_LOG_INFO("ResourceManager: Loaded shader: %s / %s", vertexFile.c_str(), fragmentFile.c_str());
        return shader;
    }

    Material* ResourceManager::CreateMaterial(const std::string& name)
    {
        // If a material with that name already exists, return it? Or allow duplicates?
        // We'll allow multiple with same name but warn.
        if (m_materials.find(name) != m_materials.end()) {
            USE_LOG_WARN("ResourceManager: Material '%s' already exists, creating another.", name.c_str());
        }

        Material* mat = new Material(name.c_str());
        m_materials[name] = std::unique_ptr<Material>(mat);
        USE_LOG_INFO("ResourceManager: Created material: %s", name.c_str());
        return mat;
    }

    Texture* ResourceManager::GetTexture(const std::string& filename) const
    {
        auto it = m_textures.find(filename);
        return (it != m_textures.end()) ? it->second.get() : nullptr;
    }

    Model* ResourceManager::GetModel(const std::string& filename) const
    {
        auto it = m_models.find(filename);
        return (it != m_models.end()) ? it->second.get() : nullptr;
    }

    SoundBuffer* ResourceManager::GetSound(const std::string& filename) const
    {
        auto it = m_sounds.find(filename);
        return (it != m_sounds.end()) ? it->second.get() : nullptr;
    }

    Shader* ResourceManager::GetShader(const std::string& name) const
    {
        auto it = m_shaders.find(name);
        return (it != m_shaders.end()) ? it->second.get() : nullptr;
    }

    Material* ResourceManager::GetMaterial(const std::string& name) const
    {
        auto it = m_materials.find(name);
        return (it != m_materials.end()) ? it->second.get() : nullptr;
    }

    void ResourceManager::UnloadTexture(const std::string& filename)
    {
        m_textures.erase(filename);
        USE_LOG_INFO("ResourceManager: Unloaded texture: %s", filename.c_str());
    }

    void ResourceManager::UnloadModel(const std::string& filename)
    {
        m_models.erase(filename);
        USE_LOG_INFO("ResourceManager: Unloaded model: %s", filename.c_str());
    }

    void ResourceManager::UnloadSound(const std::string& filename)
    {
        m_sounds.erase(filename);
        USE_LOG_INFO("ResourceManager: Unloaded sound: %s", filename.c_str());
    }

    void ResourceManager::UnloadShader(const std::string& name)
    {
        m_shaders.erase(name);
        USE_LOG_INFO("ResourceManager: Unloaded shader: %s", name.c_str());
    }

    void ResourceManager::UnloadMaterial(const std::string& name)
    {
        m_materials.erase(name);
        USE_LOG_INFO("ResourceManager: Unloaded material: %s", name.c_str());
    }

    void ResourceManager::UnloadAll()
    {
        m_textures.clear();
        m_models.clear();
        m_sounds.clear();
        m_shaders.clear();
        m_materials.clear();
        USE_LOG_INFO("ResourceManager: Unloaded all resources.");
    }

    std::string ResourceManager::MakeShaderKey(const std::string& vertex, const std::string& fragment,
                                               const std::string& geometry) const
    {
        std::string key = vertex + "|" + fragment;
        if (!geometry.empty()) key += "|" + geometry;
        return key;
    }

} // namespace USE