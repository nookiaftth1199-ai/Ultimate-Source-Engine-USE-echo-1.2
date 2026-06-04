// ============================================================
// Ultimate Source Engine - glTF Loader
// ============================================================
// Imports glTF 2.0 files using the cgltf library (MIT license).
// ============================================================

#pragma once

#include <string>

namespace USE
{
	class Model;

	class GLTFLoader
	{
	public:
		// Load a glTF file (either .gltf or .glb) into a Model.
		// Returns true on success, false on error.
		static bool Load(const std::string& filePath, Model& outModel);
	};
}