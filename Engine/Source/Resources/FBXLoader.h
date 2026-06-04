// ============================================================
// ============================================================
// Ultimate Source Engine - FBX Loader
// ============================================================
// Imports FBX files using the ufbx library (no FBX SDK needed).
// ============================================================

#pragma once

#include <string>

namespace USE
{
	class Model;

	class FBXLoader
	{
	public:
		// Load an FBX file into a Model.
		// Returns true on success, false on error.
		static bool Load(const std::string& filePath, Model& outModel);
	};
}