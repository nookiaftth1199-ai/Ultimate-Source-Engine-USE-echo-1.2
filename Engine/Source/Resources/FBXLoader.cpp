// FBXLoader.cpp – stub (FBX SDK not available)
#include "stdafx.h"
#include "FBXLoader.h"
#include "Core/Logger.h"

namespace USE
{
	bool FBXLoader::Load(const std::string& filePath, Model& outModel)
	{
		USE_LOG_WARN("FBXLoader: FBX SDK not available – cannot load '%s'.", filePath.c_str());
		return false;
	}
}