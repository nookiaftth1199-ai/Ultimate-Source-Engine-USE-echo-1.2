// ============================================================
// Ultimate Source Engine - Platform Abstraction Layer
// ============================================================
// Declares functions that must be implemented for each OS.
// ============================================================

#pragma once

#include <string>

namespace USE {
	namespace Platform {

		// Write a string to the platform's debug output.
		void DebugOutput(const std::string& msg);

		// Show a modal message box.
		void ShowMessageBox(const std::string& title, const std::string& message);

		// Return the full path to the running executable.
		std::string GetExecutablePath();

		// Return the path where user documents/data should be stored.
		std::string GetUserDocumentsPath();

		// Open a URL in the default browser.
		void OpenURL(const std::string& url);

	} // namespace Platform
} // namespace USE