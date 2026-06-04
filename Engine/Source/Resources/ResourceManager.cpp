#include "stdafx.h"
#include "ResourceManager.h"
#include "Resources/TextureLoader.h"
#include "Resources/ShaderLoader.h"
#include "Resources/MaterialLoader.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include <unordered_map>

namespace USE
{
	struct ResourceManager::Impl
	{
		FileSystem* fileSystem = nullptr;
		std::unordered_map<std::string, Texture>  textures;
		std::unordered_map<std::string, Shader>   shaders;
		std::unordered_map<std::string, Material> materials;
	};

	ResourceManager::ResourceManager() : m_impl(std::make_unique<Impl>()) {}
	ResourceManager::~ResourceManager() = default;

	bool ResourceManager::Initialize(FileSystem* fileSystem)
	{
		m_impl->fileSystem = fileSystem;
		USE_LOG_INFO("ResourceManager initialized.");
		return true;
	}

	Texture* ResourceManager::LoadTexture(const std::string& path)
	{
		auto it = m_impl->textures.find(path);
		if (it != m_impl->textures.end()) return &it->second;

		Texture tex;
		if (TextureLoader::Load(path, tex))
		{
			m_impl->textures[path] = tex;
			return &m_impl->textures[path];
		}
		return nullptr;
	}

	Shader* ResourceManager::LoadShader(const std::string& vert, const std::string& frag)
	{
		std::string key = vert + ":" + frag;
		auto it = m_impl->shaders.find(key);
		if (it != m_impl->shaders.end()) return &it->second;

		Shader shader;
		if (ShaderLoader::Load(vert, frag, shader))
		{
			m_impl->shaders[key] = shader;
			return &m_impl->shaders[key];
		}
		return nullptr;
	}

	Material* ResourceManager::LoadMaterial(const std::string& path)
	{
		auto it = m_impl->materials.find(path);
		if (it != m_impl->materials.end()) return &it->second;

		Material mat;
		if (MaterialLoader::Load(path, mat))
		{
			m_impl->materials[path] = mat;
			return &m_impl->materials[path];
		}
		return nullptr;
	}
}