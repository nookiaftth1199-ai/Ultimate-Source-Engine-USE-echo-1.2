#include "stdafx.h"
#include "ShaderLoader.h"
#include "Core/FileSystem.h"
#include "Renderer/Shader.h"
#include "Core/Logger.h"

namespace USE
{
	bool ShaderLoader::Load(const std::string& vertexPath, const std::string& pixelPath, Shader& outShader)
	{
		auto vertData = FileSystem::ReadAllBytes(vertexPath);
		auto fragData = FileSystem::ReadAllBytes(pixelPath);
		if (vertData.empty() || fragData.empty())
		{
			USE_LOG_ERROR("ShaderLoader: Could not read %s / %s", vertexPath.c_str(), pixelPath.c_str());
			return false;
		}

		std::string vertSource(vertData.begin(), vertData.end());
		std::string fragSource(fragData.begin(), fragData.end());
		return outShader.Compile(vertSource, fragSource);
	}
}