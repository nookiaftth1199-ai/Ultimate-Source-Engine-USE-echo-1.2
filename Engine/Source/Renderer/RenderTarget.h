#pragma once
#include "IRenderDevice.h"
#include "../Math/Vector4.h"

namespace USE
{
	class RenderTarget
	{
	public:
		RenderTarget() = default;
		~RenderTarget() = default;

		bool Create(IRenderDevice* device, uint32_t width, uint32_t height, TextureFormat format, bool createDepthStencil = true);
		void Destroy();

		uint32_t GetID() const { return m_id; }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		static RenderTarget Default();

	private:
		IRenderDevice* m_device = nullptr;
		uint32_t m_id = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
	};

	inline RenderTarget RenderTarget::Default() { return RenderTarget(); }
}