// ============================================================
// Ultimate Source Engine - File System Implementation
// ============================================================

#include "stdafx.h"
#include "FileSystem.h"
#include "Core/Logger.h"
#include <fstream>
#include <windows.h>

namespace USE
{
	// -----------------------------------------------------------------
	// File implementation (simple wrapper around ifstream)
	// -----------------------------------------------------------------
	File::File() = default;
	File::~File() { Close(); }

	bool File::Open(const std::string& path)
	{
		Close();
		// We'll use the FileSystem's ReadAllBytes for simplicity,
		// so this class is kept minimal for now.
		m_path = path;
		return true;
	}

	void File::Close()
	{
		m_path.clear();
	}

	bool File::IsOpen() const
	{
		return !m_path.empty();
	}

	std::vector<uint8_t> File::ReadAllBytes()
	{
		return FileSystem::ReadAllBytes(m_path);
	}

	// -----------------------------------------------------------------
	// FileSystem implementation
	// -----------------------------------------------------------------
	bool FileSystem::Initialize(const std::string& assetPath)
	{
		m_assetRoot = assetPath;
		// Ensure the path ends with a backslash.
		if (!m_assetRoot.empty() && m_assetRoot.back() != '\\' && m_assetRoot.back() != '/')
			m_assetRoot += '\\';
		USE_LOG_INFO("FileSystem initialized. Asset root: %s", m_assetRoot.c_str());
		return true;
	}

	void FileSystem::Shutdown()
	{
		USE_LOG_INFO("FileSystem shut down.");
	}

	std::string FileSystem::GetAssetPath(const std::string& relativePath) const
	{
		return m_assetRoot + relativePath;
	}

	std::vector<uint8_t> FileSystem::ReadAllBytes(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
			return {};

		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<uint8_t> data(size);
		file.read(reinterpret_cast<char*>(data.data()), size);
		return data;
	}

	bool FileSystem::Exists(const std::string& path)
	{
		DWORD attrib = GetFileAttributesA(path.c_str());
		return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
	}
}