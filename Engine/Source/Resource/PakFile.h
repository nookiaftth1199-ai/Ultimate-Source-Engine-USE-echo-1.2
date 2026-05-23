// ============================================================
// Ultimate Source Engine - Pak Archive
// ============================================================
//
// Provides read-only access to a PAK archive file (collection of files).
// Supports listing, reading, and extracting files.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace USE {

    class File; // forward declaration

    struct PakFileEntry {
        std::string filename;      // full path inside archive
        uint32_t offset;            // offset from start of pak file
        uint32_t size;              // uncompressed size
        uint32_t compressedSize;    // compressed size (if compressed)
        uint8_t  compressionMethod; // 0 = none
    };

    class PakArchive {
    public:
        PakArchive();
        ~PakArchive();

        // Open a pak file for reading. Returns true on success.
        bool Open(const std::string& filepath);

        // Close the archive.
        void Close();

        // Check if a file exists in the archive.
        bool FileExists(const std::string& filename) const;

        // Get the size of a file (uncompressed).
        int GetFileSize(const std::string& filename) const;

        // Read a file into a buffer (buffer must be large enough).
        bool ReadFile(const std::string& filename, void* buffer, size_t bufferSize);

        // Extract a file to the filesystem.
        bool ExtractFile(const std::string& filename, const std::string& outputPath);

        // List all files in the archive.
        void ListFiles(std::vector<std::string>& outFiles) const;

        // Check if archive is open.
        bool IsOpen() const { return m_isOpen; }

    private:
        std::unordered_map<std::string, PakFileEntry> m_fileMap;
        File* m_file;
        bool  m_isOpen;

        // Read the pak header and build file table.
        bool ReadHeader();
    };

} // namespace USE