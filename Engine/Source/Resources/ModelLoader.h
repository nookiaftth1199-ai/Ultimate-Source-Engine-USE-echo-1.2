// ============================================================
// Ultimate Source Engine - Model Loader
// ============================================================
#pragma once

#include <string>

namespace USE
{
	class Model;

	class ModelLoader
	{
	public:
		// Loads FBX, glTF, OBJ, and more using Assimp.
		// If Assimp is not present, returns false and logs a warning.
		static bool Load(const std::string& filePath, Model& outModel);
	};
}