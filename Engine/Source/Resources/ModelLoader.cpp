#include "stdafx.h"
#include "ModelLoader.h"
#include "Renderer/Model.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

// To enable Assimp, uncomment the line below and link assimp.lib
// #define USE_ASSIMP

#ifdef USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

namespace USE
{
	bool ModelLoader::Load(const std::string& filePath, Model& outModel)
	{
#ifdef USE_ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath,
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene || !scene->mRootNode)
		{
			USE_LOG_ERROR("ModelLoader: Assimp failed to load %s: %s",
				filePath.c_str(), importer.GetErrorString());
			return false;
		}
		// Conversion code would go here... (omitted for brevity)
		return true;
#else
		USE_LOG_WARN("ModelLoader: Assimp not enabled. To enable, define USE_ASSIMP and link assimp.");
		return false;
#endif
	}
}