#include "stdafx.h"
#include "AssimpWrapper.h"
#include "Core/Logger.h"

// #define USE_ASSIMP

#ifdef USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

namespace USE
{
	bool AssimpWrapper::LoadModel(const std::string& path, Model& outModel)
	{
#ifdef USE_ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene)
		{
			USE_LOG_ERROR("AssimpWrapper: Failed to load %s: %s", path.c_str(), importer.GetErrorString());
			return false;
		}
		// Conversion code would go here (mesh creation, materials, etc.)
		return true;
#else
		USE_LOG_WARN("AssimpWrapper: Assimp not enabled. Define USE_ASSIMP to enable.");
		return false;
#endif
	}
}