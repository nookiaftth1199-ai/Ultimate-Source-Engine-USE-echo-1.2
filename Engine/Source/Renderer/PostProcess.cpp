// ============================================================
// Ultimate Source Engine - Post-Processing Implementation
// ============================================================

#include "stdafx.h"
#include "PostProcess.h"
#include "Core/Engine.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Core/Logger.h"

namespace USE {

	// -----------------------------------------------------------------
	// BloomEffect implementation
	// -----------------------------------------------------------------

	// Shader sources (simplified GLSL 1.20)
	static const char* s_brightPassVertex =
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vUV = uv;\n"
		"    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);\n"
		"}\n";

	static const char* s_brightPassFragment =
		"uniform sampler2D source;\n"
		"uniform float threshold;\n"
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vec3 color = texture2D(source, vUV).rgb;\n"
		"    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));\n"
		"    if (luminance > threshold)\n"
		"        gl_FragColor = vec4(color, 1.0);\n"
		"    else\n"
		"        gl_FragColor = vec4(0.0);\n"
		"}\n";

	static const char* s_blurVertex =
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vUV = uv;\n"
		"    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);\n"
		"}\n";

	static const char* s_blurHorizontalFragment =
		"uniform sampler2D source;\n"
		"uniform float blurSize;\n"
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vec4 color = vec4(0.0);\n"
		"    float offsets[5] = float[5](-2.0, -1.0, 0.0, 1.0, 2.0);\n"
		"    float weights[5] = float[5](0.05, 0.25, 0.4, 0.25, 0.05);\n"
		"    for (int i = 0; i < 5; ++i) {\n"
		"        color += texture2D(source, vUV + vec2(offsets[i] * blurSize, 0.0)) * weights[i];\n"
		"    }\n"
		"    gl_FragColor = color;\n"
		"}\n";

	static const char* s_blurVerticalFragment =
		"uniform sampler2D source;\n"
		"uniform float blurSize;\n"
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vec4 color = vec4(0.0);\n"
		"    float offsets[5] = float[5](-2.0, -1.0, 0.0, 1.0, 2.0);\n"
		"    float weights[5] = float[5](0.05, 0.25, 0.4, 0.25, 0.05);\n"
		"    for (int i = 0; i < 5; ++i) {\n"
		"        color += texture2D(source, vUV + vec2(0.0, offsets[i] * blurSize)) * weights[i];\n"
		"    }\n"
		"    gl_FragColor = color;\n"
		"}\n";

	static const char* s_compositeVertex =
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vUV = uv;\n"
		"    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);\n"
		"}\n";

	static const char* s_compositeFragment =
		"uniform sampler2D original;\n"
		"uniform sampler2D bloom;\n"
		"uniform float intensity;\n"
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vec3 orig = texture2D(original, vUV).rgb;\n"
		"    vec3 bloomColor = texture2D(bloom, vUV).rgb;\n"
		"    gl_FragColor = vec4(orig + bloomColor * intensity, 1.0);\n"
		"}\n";

	BloomEffect::BloomEffect(RenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
		, m_threshold(1.0f)
		, m_intensity(0.5f)
		, m_blurSize(0.005f)
		, m_brightPassTarget(nullptr)
		, m_blurTempTarget(nullptr)
		, m_blurTarget(nullptr)
		, m_brightPassMaterial(nullptr)
		, m_blurHorizontalMaterial(nullptr)
		, m_blurVerticalMaterial(nullptr)
		, m_compositeMaterial(nullptr)
		, m_fullscreenQuad(nullptr)
	{
		CreateMaterials();
		CreateFullscreenQuad();
	}

	BloomEffect::~BloomEffect()
	{
		delete m_brightPassTarget;
		delete m_blurTempTarget;
		delete m_blurTarget;
		delete m_brightPassMaterial;
		delete m_blurHorizontalMaterial;
		delete m_blurVerticalMaterial;
		delete m_compositeMaterial;
		delete m_fullscreenQuad;
	}

	void BloomEffect::CreateMaterials()
	{
		Shader* brightShader = Shader::Create();
		brightShader->LoadFromSource(s_brightPassVertex, s_brightPassFragment);
		m_brightPassMaterial = new Material("BloomBrightPass");
		m_brightPassMaterial->SetShader(brightShader);
		m_brightPassMaterial->SetUniform("threshold", m_threshold);

		Shader* blurHShader = Shader::Create();
		blurHShader->LoadFromSource(s_blurVertex, s_blurHorizontalFragment);
		m_blurHorizontalMaterial = new Material("BloomBlurH");
		m_blurHorizontalMaterial->SetShader(blurHShader);
		m_blurHorizontalMaterial->SetUniform("blurSize", m_blurSize);

		Shader* blurVShader = Shader::Create();
		blurVShader->LoadFromSource(s_blurVertex, s_blurVerticalFragment);
		m_blurVerticalMaterial = new Material("BloomBlurV");
		m_blurVerticalMaterial->SetShader(blurVShader);
		m_blurVerticalMaterial->SetUniform("blurSize", m_blurSize);

		Shader* compShader = Shader::Create();
		compShader->LoadFromSource(s_compositeVertex, s_compositeFragment);
		m_compositeMaterial = new Material("BloomComposite");
		m_compositeMaterial->SetShader(compShader);
		m_compositeMaterial->SetUniform("intensity", m_intensity);
	}

	void BloomEffect::CreateFullscreenQuad()
	{
		float vertices[] = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f
		};
		uint32_t indices[] = { 0, 1, 2, 2, 1, 3 };
		IRenderDevice* device = m_renderSystem->GetDevice();
		m_fullscreenQuad = new Mesh();
		m_fullscreenQuad->Create(device, vertices, 4, 5 * sizeof(float), indices, 6);
	}

	void BloomEffect::CreateRenderTargets(int width, int height)
	{
		IRenderDevice* device = m_renderSystem->GetDevice();
		int halfW = width / 2;
		int halfH = height / 2;
		m_brightPassTarget = new RenderTarget();
		m_brightPassTarget->Create(device, halfW, halfH, TextureFormat::RGBA16_FLOAT, false);
		m_blurTempTarget = new RenderTarget();
		m_blurTempTarget->Create(device, halfW, halfH, TextureFormat::RGBA16_FLOAT, false);
		m_blurTarget = new RenderTarget();
		m_blurTarget->Create(device, halfW, halfH, TextureFormat::RGBA16_FLOAT, false);
	}

	void BloomEffect::OnResize(int width, int height)
	{
		delete m_brightPassTarget;
		delete m_blurTempTarget;
		delete m_blurTarget;
		CreateRenderTargets(width, height);
	}

	void BloomEffect::Apply(RenderTarget* source, RenderTarget* destination)
	{
		if (!m_enabled || !source) return;

		IRenderDevice* device = m_renderSystem->GetDevice();
		int w = source->GetWidth();
		int h = source->GetHeight();
		if (!m_brightPassTarget) CreateRenderTargets(w, h);

		device->SetRenderTarget(m_brightPassTarget);
		m_brightPassMaterial->SetTexture("source", source->GetColorTexture());
		m_fullscreenQuad->Bind(device);
		m_brightPassMaterial->Bind();
		m_fullscreenQuad->Draw(device);

		device->SetRenderTarget(m_blurTempTarget);
		m_blurHorizontalMaterial->SetTexture("source", m_brightPassTarget->GetColorTexture());
		m_blurHorizontalMaterial->Bind();
		m_fullscreenQuad->Draw(device);

		device->SetRenderTarget(m_blurTarget);
		m_blurVerticalMaterial->SetTexture("source", m_blurTempTarget->GetColorTexture());
		m_blurVerticalMaterial->Bind();
		m_fullscreenQuad->Draw(device);

		if (destination) device->SetRenderTarget(destination);
		else device->SetRenderTarget(0);
		m_compositeMaterial->SetTexture("original", source->GetColorTexture());
		m_compositeMaterial->SetTexture("bloom", m_blurTarget->GetColorTexture());
		m_compositeMaterial->SetUniform("intensity", m_intensity);
		m_compositeMaterial->Bind();
		m_fullscreenQuad->Draw(device);
	}

	// -----------------------------------------------------------------
	// ToneMappingEffect (unchanged from earlier, kept for completeness)
	// -----------------------------------------------------------------
	static const char* s_toneMapVertex =
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vUV = uv;\n"
		"    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);\n"
		"}\n";

	static const char* s_reinhardFragment =
		"uniform sampler2D source;\n"
		"uniform float exposure;\n"
		"uniform float gamma;\n"
		"varying vec2 vUV;\n"
		"void main() {\n"
		"    vec3 hdr = texture2D(source, vUV).rgb * exposure;\n"
		"    vec3 ldr = hdr / (hdr + vec3(1.0));\n"
		"    ldr = pow(ldr, vec3(1.0/gamma));\n"
		"    gl_FragColor = vec4(ldr, 1.0);\n"
		"}\n";

	ToneMappingEffect::ToneMappingEffect(RenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
		, m_operator(Operator::Reinhard)
		, m_exposure(1.0f)
		, m_gamma(2.2f)
		, m_material(nullptr)
		, m_fullscreenQuad(nullptr)
	{
		float vertices[] = { -1,-1,0,0,0, 1,-1,0,1,0, -1,1,0,0,1, 1,1,0,1,1 };
		uint32_t indices[] = { 0,1,2,2,1,3 };
		IRenderDevice* device = m_renderSystem->GetDevice();
		m_fullscreenQuad = new Mesh();
		m_fullscreenQuad->Create(device, vertices, 4, 5 * sizeof(float), indices, 6);
		UpdateShader();
	}

	ToneMappingEffect::~ToneMappingEffect()
	{
		delete m_material;
		delete m_fullscreenQuad;
	}

	void ToneMappingEffect::UpdateShader()
	{
		Shader* shader = Shader::Create();
		shader->LoadFromSource(s_toneMapVertex, s_reinhardFragment);
		delete m_material;
		m_material = new Material("ToneMapping");
		m_material->SetShader(shader);
		m_material->SetUniform("exposure", m_exposure);
		m_material->SetUniform("gamma", m_gamma);
	}

	void ToneMappingEffect::SetOperator(Operator op)
	{
		m_operator = op;
		// For brevity, only Reinhard is implemented here; you can add other shaders.
	}

	void ToneMappingEffect::OnResize(int, int) {}

	void ToneMappingEffect::Apply(RenderTarget* source, RenderTarget* destination)
	{
		if (!m_enabled || !source) return;
		IRenderDevice* device = m_renderSystem->GetDevice();
		if (destination) device->SetRenderTarget(destination);
		else device->SetRenderTarget(0);
		m_material->SetTexture("source", source->GetColorTexture());
		m_material->Bind();
		m_fullscreenQuad->Bind(device);
		m_fullscreenQuad->Draw(device);
	}

	// -----------------------------------------------------------------
	// PostProcessManager
	// -----------------------------------------------------------------
	PostProcessManager::PostProcessManager(RenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
		, m_pingPongTargets{ nullptr, nullptr }
		, m_enabled(true)
	{
	}

	PostProcessManager::~PostProcessManager()
	{
		ClearEffects();
		delete m_pingPongTargets[0];
		delete m_pingPongTargets[1];
	}

	void PostProcessManager::AddEffect(PostProcessEffect* effect)
	{
		m_effects.push_back(effect);
	}

	void PostProcessManager::RemoveEffect(PostProcessEffect* effect)
	{
		auto it = std::find(m_effects.begin(), m_effects.end(), effect);
		if (it != m_effects.end()) {
			delete *it;
			m_effects.erase(it);
		}
	}

	void PostProcessManager::ClearEffects()
	{
		for (auto* e : m_effects) delete e;
		m_effects.clear();
	}

	void PostProcessManager::Apply(RenderTarget* sceneTexture)
	{
		if (!m_enabled || m_effects.empty() || !sceneTexture) return;

		int w = sceneTexture->GetWidth();
		int h = sceneTexture->GetHeight();
		if (!m_pingPongTargets[0] || m_pingPingTargets[0]->GetWidth() != w) {
			delete m_pingPongTargets[0];
			delete m_pingPongTargets[1];
			IRenderDevice* device = m_renderSystem->GetDevice();
			m_pingPongTargets[0] = new RenderTarget();
			m_pingPongTargets[1] = new RenderTarget();
			m_pingPongTargets[0]->Create(device, w, h, TextureFormat::RGBA16_FLOAT, false);
			m_pingPongTargets[1]->Create(device, w, h, TextureFormat::RGBA16_FLOAT, false);
		}

		RenderTarget* src = sceneTexture;
		RenderTarget* dst = m_pingPongTargets[0];
		bool ping = true;

		for (size_t i = 0; i < m_effects.size(); ++i) {
			if (!m_effects[i]->IsEnabled()) continue;
			if (i == m_effects.size() - 1) {
				m_effects[i]->Apply(src, nullptr);
			}
			else {
				m_effects[i]->Apply(src, dst);
				src = dst;
				dst = (ping ? m_pingPongTargets[1] : m_pingPongTargets[0]);
				ping = !ping;
			}
		}
	}

	void PostProcessManager::OnResize(int width, int height)
	{
		for (auto* e : m_effects) e->OnResize(width, height);
		delete m_pingPongTargets[0];
		delete m_pingPongTargets[1];
		m_pingPongTargets[0] = m_pingPongTargets[1] = nullptr;
	}

	void PostProcessManager::SetEnabled(bool enabled)
	{
		m_enabled = enabled;
	}

} // namespace USE