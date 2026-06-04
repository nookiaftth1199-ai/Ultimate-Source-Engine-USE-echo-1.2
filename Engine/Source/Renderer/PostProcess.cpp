// ============================================================
// Ultimate Source Engine - Post Process Stack Implementation
// ============================================================

#include "stdafx.h"
#include "PostProcessStack.h"
#include "DepthOfField.h"
#include "MotionBlur.h"
#include "SSR.h"
#include "ColorGrading.h"
#include "Core/Logger.h"

namespace USE
{
	PostProcessStack::PostProcessStack() = default;
	PostProcessStack::~PostProcessStack() { Shutdown(); }

	bool PostProcessStack::Initialize(IRenderDevice* device, uint32_t width, uint32_t height)
	{
		if (!device)
		{
			USE_LOG_ERROR("PostProcessStack: Invalid render device.");
			return false;
		}

		m_device = device;
		m_width = width;
		m_height = height;

		// Create temporary textures for ping‑pong.
		if (!CreateTempTextures())
			return false;

		// Create and initialize each effect.
		m_dof = std::make_unique<DepthOfField>();
		if (!m_dof->Initialize(device, width, height)) return false;
		m_dof->SetEnabled(true);

		m_motionBlur = std::make_unique<MotionBlur>();
		if (!m_motionBlur->Initialize(device, width, height)) return false;
		m_motionBlur->SetEnabled(true);

		m_ssr = std::make_unique<SSR>();
		if (!m_ssr->Initialize(device, width, height)) return false;
		m_ssr->SetEnabled(true);

		m_colorGrading = std::make_unique<ColorGrading>();
		if (!m_colorGrading->Initialize(device, width, height)) return false;
		m_colorGrading->SetEnabled(true);

		// Define the processing order.
		m_effectOrder = { "SSR", "DepthOfField", "MotionBlur", "ColorGrading" };

		m_initialized = true;
		USE_LOG_INFO("PostProcessStack initialized (%u x %u).", width, height);
		return true;
	}

	void PostProcessStack::Shutdown()
	{
		if (m_device)
		{
			if (m_tempA) m_device->DestroyTexture(m_tempA);
			if (m_tempB) m_device->DestroyTexture(m_tempB);
			m_device = nullptr;
		}
		m_dof.reset();
		m_motionBlur.reset();
		m_ssr.reset();
		m_colorGrading.reset();
		m_initialized = false;
		USE_LOG_INFO("PostProcessStack shut down.");
	}

	bool PostProcessStack::CreateTempTextures()
	{
		m_tempA = m_device->CreateRenderTarget(m_width, m_height,
			TextureFormat::R8G8B8A8_UNORM, false);
		m_tempB = m_device->CreateRenderTarget(m_width, m_height,
			TextureFormat::R8G8B8A8_UNORM, false);

		if (!m_tempA || !m_tempB)
		{
			USE_LOG_ERROR("PostProcessStack: Failed to create temporary textures.");
			return false;
		}
		return true;
	}

	void PostProcessStack::Apply(uint32_t sourceTexture, uint32_t finalTarget)
	{
		if (!m_initialized || !m_device)
			return;

		uint32_t input = sourceTexture;
		uint32_t output = m_tempA;
		int ping = 0;

		for (size_t i = 0; i < m_effectOrder.size(); ++i)
		{
			const std::string& name = m_effectOrder[i];
			bool last = (i == m_effectOrder.size() - 1);

			// Determine the output target for this pass.
			uint32_t target = last ? finalTarget : output;

			if (name == "DepthOfField" && m_dof->IsEnabled())
			{
				// DepthOfField needs depth texture as well, but we'll just pass a dummy for now.
				m_dof->Apply(input, 0, target, 5.0f, 1.0f, 10.0f);
			}
			else if (name == "MotionBlur" && m_motionBlur->IsEnabled())
			{
				m_motionBlur->Apply(input, 0, 0, target, 30.0f, 16);
			}
			else if (name == "SSR" && m_ssr->IsEnabled())
			{
				// SSR needs normal and depth textures – placeholder zeros.
				m_ssr->Apply(input, 0, 0, target, Matrix4::Identity(), 64, 50.0f);
			}
			else if (name == "ColorGrading" && m_colorGrading->IsEnabled())
			{
				m_colorGrading->Apply(input, target);
			}
			else
			{
				// If the effect is disabled or unknown, just copy input to output.
				// (In a real implementation, a simple blit/pass‑through shader would be used.)
				// For now, skip.
				continue;
			}

			// Swap input/output for the next pass.
			if (!last)
			{
				input = target;
				// Toggle temporary targets.
				output = (output == m_tempA) ? m_tempB : m_tempA;
			}
		}
	}

	void PostProcessStack::SetEffectEnabled(const std::string& name, bool enabled)
	{
		if (name == "DepthOfField" && m_dof)   m_dof->SetEnabled(enabled);
		else if (name == "MotionBlur" && m_motionBlur) m_motionBlur->SetEnabled(enabled);
		else if (name == "SSR" && m_ssr)        m_ssr->SetEnabled(enabled);
		else if (name == "ColorGrading" && m_colorGrading) m_colorGrading->SetEnabled(enabled);
	}
}