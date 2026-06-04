#include "stdafx.h"
#include "STBImageWrapper.h"
#include "Core/Logger.h"

// #define USE_STB_IMAGE

#ifdef USE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"
#endif

namespace USE
{
	bool STBImageWrapper::Load(const std::string& path, ImageData& outImage)
	{
#ifdef USE_STB_IMAGE
		int w, h, comp;
		stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &comp, 4);
		if (!pixels) {
			USE_LOG_ERROR("STBImageWrapper: stb_image failed: %s", stbi_failure_reason());
			return false;
		}
		outImage.width = w;
		outImage.height = h;
		outImage.pixels.assign(pixels, pixels + w * h * 4);
		stbi_image_free(pixels);
		return true;
#else
		USE_LOG_WARN("STBImageWrapper: stb_image not enabled. Use TextureLoader instead.");
		return false;
#endif
	}
}