// ============================================================
// ============================================================
// Ultimate Source Engine – Material
// ============================================================
// Describes surface properties.
// ============================================================

#pragma once

#include <string>
#include <unordered_map>
#include "Math/Vector4.h"
#include "RenderTypes.h"   // for BlendMode

namespace USE
{
	class Texture;

	class Material
	{
	public:
		Material();
		~Material();

		// Name for identification.
		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		// Blend mode.
		void SetBlendMode(BlendMode mode) { m_blendMode = mode; }
		BlendMode GetBlendMode() const { return m_blendMode; }

		// Texture slots.
		void SetTexture(const std::string& slot, Texture* tex);
		Texture* GetTexture(const std::string& slot) const;
		uint32_t GetTextureHandle(const std::string& slot) const;   // returns GPU handle

		// Uniform parameters (albedo, roughness, metallic, etc.)
		void SetVector(const std::string& name, const Vector4& value);
		Vector4 GetVector(const std::string& name) const;

	private:
		std::string m_name;
		BlendMode   m_blendMode = BlendMode::Opaque;

		std::unordered_map<std::string, Texture*> m_textures;
		std::unordered_map<std::string, Vector4>  m_vectors;
	};
}