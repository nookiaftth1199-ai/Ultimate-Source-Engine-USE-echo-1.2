// ============================================================
// Ultimate Source Engine - Post Process Stack
// ============================================================
// Manages the full chain of screen‑space effects.
// ============================================================

#pragma once

#include "IRenderDevice.h"
#include <vector>
#include <memory>
#include <string>

namespace USE
{
	// Forward declare the concrete effects owned by this stack.
	class DepthOfField;
	class MotionBlur;
	class SSR;
	class ColorGrading;
	// Additional effects can be added here.

	class PostProcessStack
	{
	public:
		PostProcessStack();
		~PostProcessStack();

		bool Initialize(IRenderDevice* device, uint32_t width, uint32_t height);
		void Shutdown();

		// Apply all enabled effects in order, starting from sourceTexture and writing to finalTarget.
		void Apply(uint32_t sourceTexture, uint32_t finalTarget);

		// Enable / disable individual effects.
		void SetEffectEnabled(const std::string& name, bool enabled);

		// Access individual effects for fine‑tuning (e.g. parameters).
		DepthOfField*   GetDepthOfField() { return m_dof.get(); }
		MotionBlur*     GetMotionBlur() { return m_motionBlur.get(); }
		SSR*            GetSSR() { return m_ssr.get(); }
		ColorGrading*   GetColorGrading() { return m_colorGrading.get(); }

	private:
		bool CreateTempTextures();

		IRenderDevice* m_device = nullptr;
		uint32_t m_width = 0;
		uint32_t m_height = 0;

		// Ordered list of effect names (defines the processing order).
		std::vector<std::string> m_effectOrder;

		// Individual effect instances.
		std::unique_ptr<DepthOfField>   m_dof;
		std::unique_ptr<MotionBlur>     m_motionBlur;
		std::unique_ptr<SSR>            m_ssr;
		std::unique_ptr<ColorGrading>   m_colorGrading;

		// Temporary render targets for ping‑pong.
		uint32_t m_tempA = 0;
		uint32_t m_tempB = 0;

		bool m_initialized = false;
	};
}