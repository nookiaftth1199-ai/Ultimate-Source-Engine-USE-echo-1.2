// ============================================================
// Ultimate Source Engine - Font Loader
// ============================================================
// Creates a Font from a Windows system font.
// ============================================================

#pragma once

#include <string>

namespace USE
{
	class Font;

	class FontLoader
	{
	public:
		// Load a system font by name and size.
		// If fontName is empty, uses the default GUI font.
		// Returns true on success, false if the font could not be loaded.
		static bool Load(const std::string& fontName, int fontSize, Font& outFont);
	};
}