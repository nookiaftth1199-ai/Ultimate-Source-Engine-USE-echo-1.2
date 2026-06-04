#pragma once
#include <string>
namespace USE
{
	class Shader;
	class ShaderLoader
	{
	public:
		static bool Load(const std::string& vertexPath, const std::string& pixelPath, Shader& outShader);
	};
}