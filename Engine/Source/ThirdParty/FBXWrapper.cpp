#include "stdafx.h"
#include "FBXWrapper.h"
#include "Core/Logger.h"

// #define USE_FBX_SDK

#ifdef USE_FBX_SDK
#include <fbxsdk.h>   // Requires official FBX SDK (closed source)
#endif

namespace USE
{
	bool FBXWrapper::LoadModel(const std::string& path, Model& outModel)
	{
#ifdef USE_FBX_SDK
		// Real FBX SDK loading code would go here
		USE_LOG_INFO("FBXWrapper: Loading with FBX SDK...");
		return true;
#else
		USE_LOG_WARN("FBXWrapper: FBX SDK not available. Use ufbx (FBXLoader) instead.");
		return false;
#endif
	}
}