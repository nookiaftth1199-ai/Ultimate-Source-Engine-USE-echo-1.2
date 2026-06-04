// ============================================================
// Ultimate Source Engine – Resource Manager
// ============================================================
// Handles loading and caching of all engine resources.
// ============================================================

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "Renderer/Model.h"     // ✅ corrected include path

namespace USE
{
	class Texture;
	class Shader;
	class Material;
	class FileSystem;

	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		bool Initialize(FileSystem* fileSystem);

		// Load / cache a texture by file path.
		Texture* LoadTexture(const std::string& path);

		// Load / cache a shader program.
		Shader*  LoadShader(const std::string& vertexPath, const std::string& pixelPath);

		// Load / cache a material.
		Material* LoadMaterial(const std::string& path);

		// Look up an already‑loaded material.
		Material GetMaterial(const std::string& name) const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}