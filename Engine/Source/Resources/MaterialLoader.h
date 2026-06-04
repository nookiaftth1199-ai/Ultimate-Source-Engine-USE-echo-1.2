// ============================================================
// Ultimate Source Engine - Material Loader
// ============================================================
//
// Loads material definitions from JSON files// ============================================================
// Ultimate Source Engine - Material Loader
// ============================================================
#pragma once

#include <string>

namespace USE
{
	class Material;

	class MaterialLoader
	{
	public:
		// Parses a simple key‑value .mat file and fills outMaterial.
		// Supports: name, albedo, roughness, metallic, emissive, diffuseMap, normalMap
		static bool Load(const std::string& filePath, Material& outMaterial);
	};
}