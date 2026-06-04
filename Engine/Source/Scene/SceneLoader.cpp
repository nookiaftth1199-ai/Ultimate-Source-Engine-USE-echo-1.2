// ============================================================
// Ultimate Source Engine - Scene Loader Implementation
// ============================================================

#include "stdafx.h"
#include "SceneLoader.h"
#include "MapFormat.h"
#include "MapLoader.h"
#include "Scene.h"
#include "Resources/ResourceManager.h"
#include "Core/Logger.h"

namespace USE
{
	bool SceneLoader::LoadFromMap(Scene* scene, ResourceManager* resourceMgr, const std::string& path)
	{
		return MapLoader::LoadFromFile(scene, resourceMgr, path);
	}
}