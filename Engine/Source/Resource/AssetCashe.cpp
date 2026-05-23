// ============================================================
// Ultimate Source Engine - Asset Cache Implementation
// ============================================================

#include "stdafx.h"
#include "AssetCache.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Utility/CRC32.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

namespace USE {

    AssetCache::AssetCache()
        : m_initialized(false)
    {
    }

    AssetCache::~AssetCache()
    {
        Shutdown();
    }

    bool AssetCache::Initialize(const std::string& cacheRoot)
    {
        if (m_initialized) return true;

        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("AssetCache: FileSystem not available");
            return false;
        }

        m_cacheRoot = cacheRoot;
        // Ensure cache root ends with a slash
        if (!m_cacheRoot.empty() && m_cacheRoot.back() != '/' && m_cacheRoot.back() != '\\')
            m_cacheRoot += '/';

        // Create cache directory if it doesn't exist
        if (!fs->DirectoryExists(m_cacheRoot)) {
            if (!fs->CreateDirectory(m_cacheRoot)) {
                USE_LOG_ERROR("AssetCache: Failed to create cache directory: %s", m_cacheRoot.c_str());
                return false;
            }
        }

        m_initialized = true;
        USE_LOG_INFO("AssetCache initialized at: %s", m_cacheRoot.c_str());
        return true;
    }

    void AssetCache::Shutdown()
    {
        m_initialized = false;
    }

    std::string AssetCache::MakeCacheFilename(const std::string& sourcePath) const
    {
        // Use CRC32 of the source path as a hex string
        uint32_t hash = CRC32::Calculate(sourcePath.c_str(), sourcePath.length());
        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << hash;
        return ss.str();
    }

    bool AssetCache::GetCacheEntry(const std::string& sourcePath, CacheEntry& entry) const
    {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        std::string filename = MakeCacheFilename(sourcePath);
        std::string metaPath = m_cacheRoot + filename + ".meta";
        std::string dataPath = m_cacheRoot + filename + ".bin";

        // Read metadata file
        auto file = fs->OpenFile(metaPath, FILE_READ | FILE_BINARY);
        if (!file) return false;

        size_t read = file->Read(&entry.sourceFileSize, sizeof(uint64_t), 1);
        if (read != 1) { file->Close(); return false; }
        read = file->Read(&entry.sourceFileTimestamp, sizeof(uint64_t), 1);
        if (read != 1) { file->Close(); return false; }
        read = file->Read(&entry.cachedFileSize, sizeof(uint64_t), 1);
        if (read != 1) { file->Close(); return false; }

        file->Close();

        // Get file info for cached data (to verify size matches)
        FileInfo info;
        if (!fs->GetFileInfo(dataPath, info)) return false;
        if (info.size != entry.cachedFileSize) return false;

        entry.cachedFilePath = dataPath;
        return true;
    }

    bool AssetCache::HasValidCache(const std::string& sourcePath, uint64_t& outSize)
    {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        // Get source file info
        FileInfo srcInfo;
        if (!fs->GetFileInfo(sourcePath, srcInfo)) return false;

        CacheEntry entry;
        if (!GetCacheEntry(sourcePath, entry)) return false;

        // Check if source file unchanged (size and timestamp match)
        if (entry.sourceFileSize != srcInfo.size ||
            entry.sourceFileTimestamp != srcInfo.modificationTime) {
            return false;
        }

        outSize = entry.cachedFileSize;
        return true;
    }

    std::string AssetCache::GetCachedPath(const std::string& sourcePath)
    {
        if (!m_initialized) return "";
        CacheEntry entry;
        if (!GetCacheEntry(sourcePath, entry)) return "";
        return entry.cachedFilePath;
    }

    bool AssetCache::StoreAsset(const std::string& sourcePath,
                                 const void* data, size_t dataSize)
    {
        if (!m_initialized) {
            USE_LOG_ERROR("AssetCache::StoreAsset: Cache not initialized");
            return false;
        }

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        // Get source file info
        FileInfo srcInfo;
        if (!fs->GetFileInfo(sourcePath, srcInfo)) {
            USE_LOG_ERROR("AssetCache: Cannot get info for source file: %s", sourcePath.c_str());
            return false;
        }

        std::string filename = MakeCacheFilename(sourcePath);
        std::string dataPath = m_cacheRoot + filename + ".bin";
        std::string metaPath = m_cacheRoot + filename + ".meta";

        // Write cooked data
        auto outFile = fs->OpenFile(dataPath, FILE_WRITE | FILE_BINARY | FILE_TRUNCATE);
        if (!outFile) {
            USE_LOG_ERROR("AssetCache: Failed to create cache file: %s", dataPath.c_str());
            return false;
        }
        size_t written = outFile->Write(data, 1, dataSize);
        outFile->Close();
        if (written != dataSize) {
            USE_LOG_ERROR("AssetCache: Failed to write all data to cache");
            fs->DeleteFile(dataPath);
            return false;
        }

        // Write metadata
        CacheEntry entry;
        entry.sourceFileSize = srcInfo.size;
        entry.sourceFileTimestamp = srcInfo.modificationTime;
        entry.cachedFileSize = dataSize;
        entry.cachedFilePath = dataPath;

        auto metaFile = fs->OpenFile(metaPath, FILE_WRITE | FILE_BINARY | FILE_TRUNCATE);
        if (!metaFile) {
            USE_LOG_ERROR("AssetCache: Failed to create metadata file: %s", metaPath.c_str());
            fs->DeleteFile(dataPath);
            return false;
        }
        metaFile->Write(&entry.sourceFileSize, sizeof(uint64_t), 1);
        metaFile->Write(&entry.sourceFileTimestamp, sizeof(uint64_t), 1);
        metaFile->Write(&entry.cachedFileSize, sizeof(uint64_t), 1);
        metaFile->Close();

        USE_LOG_INFO("AssetCache: Stored cached asset for %s", sourcePath.c_str());
        return true;
    }

    bool AssetCache::LoadCachedAsset(const std::string& sourcePath,
                                      std::vector<uint8_t>& outData)
    {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        CacheEntry entry;
        if (!GetCacheEntry(sourcePath, entry)) return false;

        // Verify source file still matches (optional, but we already did in HasValidCache)
        // We'll trust that the caller used HasValidCache, or just load anyway.

        auto file = fs->OpenFile(entry.cachedFilePath, FILE_READ | FILE_BINARY);
        if (!file) return false;

        outData.resize(entry.cachedFileSize);
        size_t read = file->Read(outData.data(), 1, entry.cachedFileSize);
        file->Close();

        if (read != entry.cachedFileSize) {
            USE_LOG_ERROR("AssetCache: Failed to read cached file: %s", entry.cachedFilePath.c_str());
            return false;
        }

        return true;
    }

    bool AssetCache::RemoveCachedAsset(const std::string& sourcePath)
    {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        std::string filename = MakeCacheFilename(sourcePath);
        std::string dataPath = m_cacheRoot + filename + ".bin";
        std::string metaPath = m_cacheRoot + filename + ".meta";

        bool ok = true;
        if (!fs->DeleteFile(dataPath)) ok = false;
        if (!fs->DeleteFile(metaPath)) ok = false;

        return ok;
    }

    bool AssetCache::Clear()
    {
        if (!m_initialized) return false;

        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;

        std::vector<FileInfo> files;
        if (!fs->ListDirectory(m_cacheRoot, files, false))
            return false;

        for (const auto& fi : files) {
            if (fi.name.size() > 4 && (fi.name.substr(fi.name.size()-4) == ".bin" ||
                                        fi.name.substr(fi.name.size()-5) == ".meta")) {
                fs->DeleteFile(fi.fullPath);
            }
        }

        return true;
    }

} // namespace USE