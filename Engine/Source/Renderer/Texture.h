// ============================================================
// Ultimate Source Engine - Texture
// ============================================================
// Manages a single GPU texture resource.
// ============================================================
// Texture.h
#pragma once

#include "IRenderDevice.h"   // <--- added this line
#include <cstdint>
#include <string>

namespace USE
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		bool Create(uint32_t width, uint32_t height, TextureFormat format, const void* data = nullptr);
		bool LoadFromFile(const std::string& filePath);
		void Update(const void* data, uint32_t mipLevel = 0);
		void Bind(uint32_t slot) const;

		uint32_t GetWidth()  const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
		uint32_t GetID()     const { return m_id; }
		bool IsValid()       const { return m_id != 0; }
		void Destroy();

	private:
		IRenderDevice* m_device = nullptr;
		uint32_t m_id = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		TextureFormat m_format = TextureFormat::R8G8B8A8_UNORM;
	};
}