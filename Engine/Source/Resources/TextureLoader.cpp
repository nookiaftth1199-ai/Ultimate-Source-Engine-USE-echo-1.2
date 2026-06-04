#include "stdafx.h"
#include "TextureLoader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace USE
{
	bool TextureLoader::Load(const std::string& filePath, Texture& outTexture)
	{
		// Convert path to wide string
		int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
		if (wideLen == 0) return false;
		std::wstring wpath(wideLen, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wpath[0], wideLen);

		Gdiplus::Bitmap bmp(wpath.c_str());
		if (bmp.GetLastStatus() != Gdiplus::Ok)
		{
			USE_LOG_ERROR("TextureLoader: Cannot open %s", filePath.c_str());
			return false;
		}

		int width = bmp.GetWidth();
		int height = bmp.GetHeight();
		std::vector<uint8_t> pixels(width * height * 4);
		Gdiplus::Rect rect(0, 0, width, height);
		Gdiplus::BitmapData bmpData;
		bmp.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);
		uint8_t* src = static_cast<uint8_t*>(bmpData.Scan0);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int srcIdx = (y * bmpData.Stride) + x * 4;
				int dstIdx = (y * width + x) * 4;
				pixels[dstIdx + 0] = src[srcIdx + 2]; // R
				pixels[dstIdx + 1] = src[srcIdx + 1]; // G
				pixels[dstIdx + 2] = src[srcIdx + 0]; // B
				pixels[dstIdx + 3] = src[srcIdx + 3]; // A
			}
		}
		bmp.UnlockBits(&bmpData);

		return outTexture.Create(width, height, TextureFormat::R8G8B8A8_UNORM, pixels.data());
	}
}