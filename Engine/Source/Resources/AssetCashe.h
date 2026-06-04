// ============================================================
// Ultimate Source Engine - Asset Cache
// ============================================================
//
// Provides a persistent disk cache for cooked asset data.
// Speeds up loading by storing processed assets (e.g., DDS textures,
// optimized meshes) alongside the original assets.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <cstdint>

namespace USE {

    // Asset cache entry metadata
    struct CacheEntry {
        uint64_t sourceFileSize;
        uint64_t sourceFileTimestamp; // last write time
        uint64_t cachedFileSize;
        std::string cachedFilePath;   // relative to cache root
    };

    class AssetCache {
    public:
        AssetCache();
        ~AssetCache();

        // Initialize cache with a root directory (default: "Cache/")
        bool Initialize(const std::string& cacheRoot = "Cache/");
        void Shutdown();

        // Check if an asset is cached and still valid (source file unchanged)
        bool HasValidCache(const std::string& sourcePath, uint64_t& outSize);

        // Get the cached file path for a source asset (returns empty if not cached)
        std::string GetCachedPath(const std::string& sourcePath);

        // Store a cooked asset into the cache
        bool StoreAsset(const std::string& sourcePath,
                        const void* data, size_t dataSize);

        // Load a cooked asset from cache into a buffer
        bool LoadCachedAsset(const std::string& sourcePath,
                             std::vector<uint8_t>& outData);

        // Remove a cached asset
        bool RemoveCachedAsset(const std::string& sourcePath);

        // Clear entire cache
        bool Clear();

    private:
        std::string m_cacheRoot;
        bool        m_initialized;

        // Generate a unique cache filename from a source path (e.g., hash)
        std::string MakeCacheFilename(const std::string& sourcePath) const;

        // Get metadata for a source path (if cache exists)
        bool GetCacheEntry(const std::string& sourcePath, CacheEntry& entry) const;

        // Write metadata for a cached asset
        bool WriteMetadata(const std::string& sourcePath,
                           const CacheEntry& entry) const;
    };

} // namespace USE