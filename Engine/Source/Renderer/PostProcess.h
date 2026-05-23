// ============================================================
// Ultimate Source Engine - Post-Processing
// ============================================================
//
// Provides a flexible post-processing pipeline with chainable effects.
// Includes bloom, tone mapping, depth of field, motion blur, and color grading.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include <vector>

namespace USE {

	// Forward declarations
	class RenderSystem;
	class Camera;

	// -----------------------------------------------------------------
	// Base class for all post-processing effects
	// -----------------------------------------------------------------
	class PostProcessEffect {
	public:
		virtual ~PostProcessEffect() = default;

		// Apply the effect to the source render target, writing to destination.
		// If destination is nullptr, the effect should render to the backbuffer.
		virtual void Apply(RenderTarget* source, RenderTarget* destination) = 0;

		// Called when the viewport size changes (to recreate internal resources)
		virtual void OnResize(int width, int height) {}

		// Enable/disable the effect
		void SetEnabled(bool enabled) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		// Get effect name (for debugging)
		virtual const char* GetName() const = 0;

	protected:
		bool m_enabled = true;
	};

	// -----------------------------------------------------------------
	// Bloom effect (simplified: extract bright parts, blur, add to original)
	// -----------------------------------------------------------------
	class BloomEffect : public PostProcessEffect {
	public:
		BloomEffect(RenderSystem* renderSystem);
		~BloomEffect();

		void Apply(RenderTarget* source, RenderTarget* destination) override;
		void OnResize(int width, int height) override;
		const char* GetName() const override { return "Bloom"; }

		void SetThreshold(float threshold) { m_threshold = threshold; }
		void SetIntensity(float intensity) { m_intensity = intensity; }
		void SetBlurSize(float size) { m_blurSize = size; }

	private:
		RenderSystem* m_renderSystem;
		float m_threshold;
		float m_intensity;
		float m_blurSize;

		RenderTarget* m_brightPassTarget;
		RenderTarget* m_blurTempTarget;
		RenderTarget* m_blurTarget;

		Material* m_brightPassMaterial;
		Material* m_blurHorizontalMaterial;
		Material* m_blurVerticalMaterial;
		Material* m_compositeMaterial;

		Mesh* m_fullscreenQuad;

		void CreateMaterials();
		void CreateRenderTargets(int width, int height);
	};

	// -----------------------------------------------------------------
	// Tone mapping effect (HDR to LDR)
	// -----------------------------------------------------------------
	class ToneMappingEffect : public PostProcessEffect {
	public:
		enum class Operator {
			Reinhard,
			Uncharted2,
			ACES
		};

		ToneMappingEffect(RenderSystem* renderSystem);
		~ToneMappingEffect();

		void Apply(RenderTarget* source, RenderTarget* destination) override;
		void OnResize(int width, int height) override;
		const char* GetName() const override { return "ToneMapping"; }

		void SetOperator(Operator op);
		void SetExposure(float exposure) { m_exposure = exposure; }
		void SetGamma(float gamma) { m_gamma = gamma; }

	private:
		RenderSystem* m_renderSystem;
		Operator m_operator;
		float m_exposure;
		float m_gamma;

		Material* m_material;
		Mesh* m_fullscreenQuad;

		void UpdateShader();
	};

	// -----------------------------------------------------------------
	// Depth of Field effect (bokeh or Gaussian)
	// -----------------------------------------------------------------
	class DepthOfFieldEffect : public PostProcessEffect {
	public:
		DepthOfFieldEffect(RenderSystem* renderSystem);
		~DepthOfFieldEffect();

		void Apply(RenderTarget* source, RenderTarget* destination) override;
		void OnResize(int width, int height) override;
		const char* GetName() const override { return "DepthOfField"; }

		void SetFocusDistance(float dist) { m_focusDistance = dist; }
		void SetFocusRange(float range) { m_focusRange = range; }
		void SetBlurAmount(float amount) { m_blurAmount = amount; }

	private:
		RenderSystem* m_renderSystem;
		float m_focusDistance;
		float m_focusRange;
		float m_blurAmount;

		RenderTarget* m_tempTarget;
		Material* m_dofMaterial;
		Mesh* m_fullscreenQuad;

		void CreateMaterials();
	};

	// -----------------------------------------------------------------
	// Motion Blur effect (based on velocity buffer)
	// -----------------------------------------------------------------
	class MotionBlurEffect : public PostProcessEffect {
	public:
		MotionBlurEffect(RenderSystem* renderSystem);
		~MotionBlurEffect();

		void Apply(RenderTarget* source, RenderTarget* destination) override;
		void OnResize(int width, int height) override;
		const char* GetName() const override { return "MotionBlur"; }

		void SetStrength(float strength) { m_strength = strength; }
		void SetSamples(int samples) { m_samples = samples; }

	private:
		RenderSystem* m_renderSystem;
		float m_strength;
		int m_samples;

		Material* m_motionBlurMaterial;
		Mesh* m_fullscreenQuad;

		void CreateMaterial();
	};

	// -----------------------------------------------------------------
	// Color Grading effect (LUT‑based)
	// -----------------------------------------------------------------
	class ColorGradingEffect : public PostProcessEffect {
	public:
		ColorGradingEffect(RenderSystem* renderSystem);
		~ColorGradingEffect();

		void Apply(RenderTarget* source, RenderTarget* destination) override;
		void OnResize(int width, int height) override;
		const char* GetName() const override { return "ColorGrading"; }

		void SetLUTTexture(Texture* lut) { m_lutTexture = lut; }
		void SetIntensity(float intensity) { m_intensity = intensity; }

	private:
		RenderSystem* m_renderSystem;
		Texture* m_lutTexture;
		float m_intensity;

		Material* m_colorGradingMaterial;
		Mesh* m_fullscreenQuad;

		void CreateMaterial();
	};

	// -----------------------------------------------------------------
	// PostProcessManager - chains multiple effects together
	// -----------------------------------------------------------------
	class PostProcessManager {
	public:
		PostProcessManager(RenderSystem* renderSystem);
		~PostProcessManager();

		// Add an effect (manager takes ownership)
		void AddEffect(PostProcessEffect* effect);

		// Remove and delete an effect (by pointer)
		void RemoveEffect(PostProcessEffect* effect);

		// Clear all effects
		void ClearEffects();

		// Apply the whole chain to the input scene texture.
		// The final result is written to the backbuffer (or provided target).
		void Apply(RenderTarget* sceneTexture);

		// Resize internal render targets when viewport changes
		void OnResize(int width, int height);

		// Enable/disable all effects (useful for debugging)
		void SetEnabled(bool enabled);
		bool IsEnabled() const { return m_enabled; }

	private:
		RenderSystem* m_renderSystem;
		std::vector<PostProcessEffect*> m_effects;
		RenderTarget* m_pingPongTargets[2];
		bool m_enabled;
	};

} // namespace USE