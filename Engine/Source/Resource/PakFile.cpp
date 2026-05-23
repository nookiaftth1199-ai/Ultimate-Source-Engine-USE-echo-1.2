// ============================================================
// Ultimate Source Engine - Pak Archive Implementation
// ============================================================

#include "stdafx.h"
#include "PakFile.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"

namespace USE {

    // Pak header (binary format)
    struct PakHeader {
        char     magic[4];        // 'P', 'A', 'K', '0'
        uint32_t version;          // 1
        uint32_t fileCount;        // number of files
        uint32_t fileTableOffset;  // offset from start of file to file table
    };

    // File entry (on disk, fixed size for simplicity)
    struct PakDiskEntry {
        char     filename[256];    // null-terminated
        uint32_t offset;
        uint32_t size;
        uint32_t compressedSize;
        uint8_t  compressionMethod;
    };

    PakArchive::PakArchive()
        : m_file(nullptr)
        , m_isOpen(false)
    {
    }

    PakArchive::~PakArchive()
    {
        Close();
    }

    bool PakArchive::Open(const std::string& filepath)
    {
        Close();

        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("PakArchive: FileSystem not available");
            return false;
        }

        m_file = fs->OpenFile(filepath, FILE_READ | FILE_BINARY);
        if (!m_file) {
            USE_LOG_ERROR("PakArchive: Could not open file: %s", filepath.c_str());
            return false;
        }

        if (!ReadHeader()) {
            Close();
            return false;
        }

        m_isOpen = true;
        USE_LOG_INFO("PakArchive: Opened %s (%u files)", filepath.c_str(), (uint32_t)m_fileMap.size());
        return true;
    }

    void PakArchive::Close()
    {
        if (m_file) {
            m_file->Close();
            delete m_file;
            m_file = nullptr;
        }
        m_fileMap.clear();
        m_isOpen = false;
    }

    bool PakArchive::ReadHeader()
    {
        // Read header
        PakHeader header;
        if (m_file->Read(&header, sizeof(PakHeader), 1) != 1) {
            USE_LOG_ERROR("PakArchive: Failed to read header");
            return false;
        }

        // Check magic
        if (header.magic[0] != 'P' || header.magic[1] != 'A' ||
            header.magic[2] != 'K' || header.magic[3] != '0') {
            USE_LOG_ERROR("PakArchive: Invalid magic (not a PAK file)");
            return false;
        }

        if (header.version != 1) {
            USE_LOG_ERROR("PakArchive: Unsupported version %u", header.version);
            return false;
        }

        // Seek to file table
        if (!m_file->Seek(header.fileTableOffset, SEEK_BEGIN)) {
            USE_LOG_ERROR("PakArchive: Failed to seek to file table");
            return false;
        }

        // Read entries
        for (uint32_t i = 0; i < header.fileCount; ++i) {
            PakDiskEntry diskEntry;
            if (m_file->Read(&diskEntry, sizeof(PakDiskEntry), 1) != 1) {
                USE_LOG_ERROR("PakArchive: Failed to read entry %u", i);
                return false;
            }

            // Ensure null termination
            diskEntry.filename[sizeof(diskEntry.filename) - 1] = '\0';

            PakFileEntry entry;
            entry.filename = diskEntry.filename;
            entry.offset = diskEntry.offset;
            entry.size = diskEntry.size;
            entry.compressedSize = diskEntry.compressedSize;
            entry.compressionMethod = diskEntry.compressionMethod;

            m_fileMap[entry.filename] = entry;
        }

        return true;
    }

    bool PakArchive::FileExists(const std::string& filename) const
    {
        return m_fileMap.find(filename) != m_fileMap.end();
    }

    int PakArchive::GetFileSize(const std::string& filename) const
    {
        auto it = m_fileMap.find(filename);
        if (it != m_fileMap.end())
            return it->second.size;
        return -1;
    }

    bool PakArchive::ReadFile(const std::string& filename, void* buffer, size_t bufferSize)
    {
        auto it = m_fileMap.find(filename);
        if (it == m_fileMap.end()) {
            USE_LOG_ERROR("PakArchive: File not found: %s", filename.c_str());
            return false;
        }

        const PakFileEntry& entry = it->second;

        if (bufferSize < entry.size) {
            USE_LOG_ERROR("PakArchive: Buffer too small for %s (need %u)", filename.c_str(), entry.size);
            return false;
        }

        // Seek to offset
        if (!m_file->Seek(entry.offset, SEEK_BEGIN)) {
            USE_LOG_ERROR("PakArchive: Failed to seek to file data for %s", filename.c_str());
            return false;
        }

        // Read data
        size_t read = m_file->Read(buffer, 1, entry.size);
        if (read != entry.size) {
            USE_LOG_ERROR("PakArchive: Failed to read file data for %s", filename.c_str());
            return false;
        }

        return true;
    }

    bool PakArchive::ExtractFile(const std::string& filename, const std::string& outputPath)
    {
        auto it = m_fileMap.find(filename);
        if (it == m_fileMap.end()) {
            USE_LOG_ERROR("PakArchive: File not found for extraction: %s", filename.c_str());
            return false;
        }

        const PakFileEntry& entry = it->second;

        // Read data into memory
        std::vector<uint8_t> data(entry.size);
        if (!ReadFile(filename, data.data(), data.size())) {
            return false;
        }

        // Write to output file
        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        auto outFile = fs->OpenFile(outputPath, FILE_WRITE | FILE_BINARY | FILE_TRUNCATE);
        if (!outFile) {
            USE_LOG_ERROR("PakArchive: Cannot create output file: %s", outputPath.c_str());
            return false;
        }

        size_t written = outFile->Write(data.data(), 1, data.size());
        outFile->Close();

        if (written != data.size()) {
            USE_LOG_ERROR("PakArchive: Failed to write all data to %s", outputPath.c_str());
            fs->DeleteFile(outputPath);
            return false;
        }

        return true;
    }

    void PakArchive::ListFiles(std::vector<std::string>& outFiles) const
    {
        outFiles.clear();
        outFiles.reserve(m_fileMap.size());
        for (const auto& pair : m_fileMap) {
            outFiles.push_back(pair.first);
        }
    }

} // namespace USE