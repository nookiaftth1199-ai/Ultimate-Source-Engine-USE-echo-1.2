#pragma once
#include <string>
#include "Renderer/Texture.h"

namespace USE
{
	class TextureLoader
	{
	public:
		static bool Load(const std::string& filePath, Texture& outTexture);
	};
}