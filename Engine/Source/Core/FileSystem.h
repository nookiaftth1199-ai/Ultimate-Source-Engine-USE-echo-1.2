// ============================================================
// Ultimate Source Engine - File System
// ============================================================
// Handles file I/O and path resolution.
// ============================================================

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace USE
{
	class File
	{
	public:
		File();
		~File();

		bool Open(const std::string& path);
		void Close();
		bool IsOpen() const;

		// Read the entire file into a vector of bytes.
		std::vector<uint8_t> ReadAllBytes();

	private:
		void* m_handle = nullptr;   // platform‑specific file handle
		std::string m_path;
	};

	class FileSystem
	{
	public:
		// Initialise the file system. assetPath is the root directory
		// where game assets are stored.
		bool Initialize(const std::string& assetPath);
		void Shutdown();

		// Resolve a relative asset path to a full path.
		std::string GetAssetPath(const std::string& relativePath) const;

		// Convenience: read an entire file into memory in one call.
		static std::vector<uint8_t> ReadAllBytes(const std::string& path);

		// Check if a file exists.
		static bool Exists(const std::string& path);

	private:
		std::string m_assetRoot;
	};
}