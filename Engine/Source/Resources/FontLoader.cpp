// FontLoader.cpp – loads system fonts using Windows GDI+
#include "stdafx.h"
#include "FontLoader.h"
#include "UI/Font.h"
#include "Renderer/Texture.h"
#include "Core/Logger.h"

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace USE
{
	// Helper: convert narrow UTF‑8 string to wide (for GDI+).
	static std::wstring ToWide(const std::string& utf8)
	{
		if (utf8.empty()) return L"";
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
		if (len <= 0) return L"";
		std::wstring w(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &w[0], len);
		return w;
	}

	bool FontLoader::Load(const std::string& fontName, int fontSize, Font& outFont)
	{
		return outFont.Create(fontName, fontSize);
	}
}