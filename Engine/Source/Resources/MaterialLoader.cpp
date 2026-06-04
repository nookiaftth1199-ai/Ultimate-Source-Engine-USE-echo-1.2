#include "stdafx.h"
#include "MaterialLoader.h"
#include "Renderer/Material.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include <sstream>

namespace USE
{
	static Vector4 ParseColor(const std::string& str)
	{
		float r = 1, g = 1, b = 1, a = 1;
		sscanf_s(str.c_str(), "%f %f %f %f", &r, &g, &b, &a);
		return Vector4(r, g, b, a);
	}

	bool MaterialLoader::Load(const std::string& filePath, Material& outMaterial)
	{
		auto data = FileSystem::ReadAllBytes(filePath);
		if (data.empty())
		{
			USE_LOG_ERROR("MaterialLoader: Cannot open %s", filePath.c_str());
			return false;
		}

		std::string content(data.begin(), data.end());
		std::istringstream stream(content);
		std::string line;
		while (std::getline(stream, line))
		{
			// Trim
			size_t start = line.find_first_not_of(" \t\r\n");
			if (start == std::string::npos) continue;
			size_t end = line.find_last_not_of(" \t\r\n");
			line = line.substr(start, end - start + 1);
			if (line.empty() || line[0] == '#') continue;

			auto eq = line.find('=');
			if (eq == std::string::npos) continue;
			std::string key = line.substr(0, eq);
			std::string val = line.substr(eq + 1);
			// Trim key
			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			val.erase(0, val.find_first_not_of(" \t"));
			val.erase(val.find_last_not_of(" \t") + 1);

			if (key == "name")        outMaterial.SetName(val);
			else if (key == "albedo") outMaterial.SetVector("albedo", ParseColor(val));
			else if (key == "roughness") outMaterial.SetVector("roughness", Vector4(std::stof(val)));
			else if (key == "metallic")  outMaterial.SetVector("metallic", Vector4(std::stof(val)));
			else if (key == "emissive")  outMaterial.SetVector("emissive", ParseColor(val));
			else if (key == "diffuseMap") outMaterial.SetTexture("diffuse", nullptr); // texture loading deferred
			else if (key == "normalMap")  outMaterial.SetTexture("normal", nullptr);
		}
		return true;
	}
}