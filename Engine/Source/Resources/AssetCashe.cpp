#include "stdafx.h"
#include "AssetCache.h"
#include "Core/Logger.h"
#include "Core/FileSystem.h"

namespace USE
{
	AssetCache::AssetCache() = default;
	AssetCache::~AssetCache() { Clear(); }

	bool AssetCache::Load(const std::string& path)
	{
		auto it = m_cache.find(path);
		if (it != m_cache.end())
		{
			it->second.refCount++;
			return true;
		}

		std::vector<uint8_t> data = FileSystem::ReadAllBytes(path);
		if (data.empty())
		{
			USE_LOG_ERROR("AssetCache: Failed to read '%s'", path.c_str());
			return false;
		}

		CachedAsset asset;
		asset.path = path;
		asset.data = std::move(data);
		asset.refCount = 1;
		m_cache[path] = std::move(asset);

		USE_LOG_INFO("AssetCache: Loaded '%s' (%zu bytes)", path.c_str(), m_cache[path].data.size());
		return true;
	}

	void AssetCache::Unload(const std::string& path)
	{
		auto it = m_cache.find(path);
		if (it == m_cache.end()) return;
		it->second.refCount--;
		if (it->second.refCount == 0)
		{
			USE_LOG_INFO("AssetCache: Unloaded '%s'", path.c_str());
			m_cache.erase(it);
		}
	}

	const std::vector<uint8_t>* AssetCache::Get(const std::string& path) const
	{
		auto it = m_cache.find(path);
		return (it != m_cache.end()) ? &it->second.data : nullptr;
	}

	bool AssetCache::IsCached(const std::string& path) const
	{
		return m_cache.find(path) != m_cache.end();
	}

	void AssetCache::AddRef(const std::string& path)
	{
		auto it = m_cache.find(path);
		if (it != m_cache.end()) it->second.refCount++;
	}

	void AssetCache::Release(const std::string& path) { Unload(path); }

	void AssetCache::Clear()
	{
		m_cache.clear();
		USE_LOG_INFO("AssetCache: Cleared all entries.");
	}
}