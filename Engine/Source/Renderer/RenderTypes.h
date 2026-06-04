// ============================================================
// Ultimate Source Engine – Render Types
// ============================================================
// Common types shared across the renderer.
// ============================================================

#pragma once

#include "Math/Color.h"          // for Color
#include "IRenderDevice.h"       // for TextureFormat, ShaderType, VertexElement, etc.

namespace USE
{
	// Blend mode enum (was missing)
	enum class BlendMode
	{
		Opaque,
		AlphaBlend,
		Additive,
		Multiply
	};

	// Additional renderer‑specific constants can live here.
	constexpr int MAX_LIGHTS = 256;
}