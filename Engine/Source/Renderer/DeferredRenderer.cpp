// ============================================================
// Ultimate Source Engine - Deferred Renderer Implementation
// ============================================================

#include "stdafx.h"
#include "DeferredRenderer.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Scene/Scene.h"
#include "Entity/Components/RenderComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Math/MathUtils.h"

namespace USE {

    // -----------------------------------------------------------------
    // Simple shader sources (GLSL 1.20). G‑buffer geometry pass.
    // -----------------------------------------------------------------
    static const char* s_geometryVertex =
        "uniform mat4 modelViewProj;\n"
        "attribute vec3 position;\n"
        "attribute vec3 normal;\n"
        "attribute vec2 texCoord;\n"
        "attribute vec3 tangent;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vNormal;\n"
        "void main() {\n"
        "    vTexCoord = texCoord;\n"
        "    vNormal = normal;\n"
        "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
        "}\n";

    static const char* s_geometryFragment =
        "uniform sampler2D albedoMap;\n"
        "uniform float metallic;\n"
        "uniform float roughness;\n"
        "varying vec2 vTexCoord;\n"
        "varying vec3 vNormal;\n"
        "void main() {\n"
        "    vec3 albedo = texture2D(albedoMap, vTexCoord).rgb;\n"
        "    gl_FragData[0] = vec4(albedo, roughness);\n"
        "    gl_FragData[1] = vec4(normalize(vNormal), metallic);\n"
        "    // world position could be stored in a third target; for now we reconstruct from depth.\n"
        "}\n";

    // Lighting pass (full‑screen quad) – simplified, single directional light.
    static const char* s_lightingVertex =
        "varying vec2 vUV;\n"
        "void main() {\n"
        "    vUV = uv;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    static const char* s_lightingFragment =
        "uniform sampler2D albedoSpec;\n"
        "uniform sampler2D normalMetal;\n"
        "uniform vec3 lightDir;\n"
        "uniform vec3 lightColor;\n"
        "uniform float ambient;\n"
        "varying vec2 vUV;\n"
        "void main() {\n"
        "    vec4 albedoSpec = texture2D(albedoSpec, vUV);\n"
        "    vec4 normalMetal = texture2D(normalMetal, vUV);\n"
        "    vec3 albedo = albedoSpec.rgb;\n"
        "    float roughness = albedoSpec.a;\n"
        "    vec3 N = normalize(normalMetal.rgb);\n"
        "    float metalness = normalMetal.a;\n"
        "    vec3 L = normalize(lightDir);\n"
        "    float diff = max(dot(N, L), 0.0);\n"
        "    vec3 diffuse = albedo * diff * lightColor;\n"
        "    vec3 ambientColor = albedo * ambient;\n"
        "    gl_FragColor = vec4(ambientColor + diffuse, 1.0);\n"
        "}\n";

    // -----------------------------------------------------------------
    DeferredRenderer::DeferredRenderer(RenderSystem* renderer)
        : m_renderer(renderer)
        , m_enabled(true)
        , m_width(0)
        , m_height(0)
        , m_depthTarget(nullptr)
        , m_fullscreenQuad(nullptr)
        , m_geometryShader(nullptr)
        , m_lightingShader(nullptr)
    {
        for (int i = 0; i < 4; ++i) m_gBuffer[i] = nullptr;
    }

    DeferredRenderer::~DeferredRenderer()
    {
        Shutdown();
    }

    bool DeferredRenderer::Initialize(int width, int height)
    {
        Shutdown();
        m_width = width;
        m_height = height;

        IRenderDevice* device = m_renderer->GetDevice();
        if (!device) return false;

        // Create G‑buffer targets (multiple render targets)
        // Target 0: RGBA8 – albedo (RGB) + roughness (A)
        // Target 1: RGBA8 – normal (RGB) + metalness (A)
        // Target 2: optional – world position (RGBA16F) or left empty.
        // Depth/stencil target is separate.
        m_gBuffer[0] = new RenderTarget();
        m_gBuffer[1] = new RenderTarget();
        m_gBuffer[2] = new RenderTarget();  // world position (optional)
        m_gBuffer[3] = nullptr;

        if (!m_gBuffer[0]->Create(device, width, height, TextureFormat::RGBA8_UNORM, false) ||
            !m_gBuffer[1]->Create(device, width, height, TextureFormat::RGBA8_UNORM, false) ||
            !m_gBuffer[2]->Create(device, width, height, TextureFormat::RGBA16_FLOAT, false)) {
            USE_LOG_ERROR("DeferredRenderer: Failed to create G‑buffer targets.");
            return false;
        }

        // Depth target (shared, also used as render target for depth)
        m_depthTarget = new RenderTarget();
        if (!m_depthTarget->Create(device, width, height, TextureFormat::D24_UNORM_S8_UINT, false)) {
            USE_LOG_ERROR("DeferredRenderer: Failed to create depth target.");
            return false;
        }

        // Create shaders
        if (!CreateShaders()) {
            USE_LOG_ERROR("DeferredRenderer: Failed to create shaders.");
            return false;
        }

        // Create fullscreen quad for lighting pass
        CreateFullscreenQuad();

        USE_LOG_INFO("DeferredRenderer initialized with %dx%d G‑buffer.", width, height);
        return true;
    }

    void DeferredRenderer::Shutdown()
    {
        for (int i = 0; i < 4; ++i) {
            delete m_gBuffer[i];
            m_gBuffer[i] = nullptr;
        }
        delete m_depthTarget;
        m_depthTarget = nullptr;

        delete m_fullscreenQuad;
        m_fullscreenQuad = nullptr;

        delete m_geometryShader;
        m_geometryShader = nullptr;
        delete m_lightingShader;
        m_lightingShader = nullptr;
    }

    void DeferredRenderer::Resize(int width, int height)
    {
        if (m_width == width && m_height == height) return;
        Shutdown();
        Initialize(width, height);
    }

    void DeferredRenderer::GeometryPass()
    {
        if (!m_enabled) return;

        // Get all renderable entities (simplified – assume we have a scene with world)
        // For each entity with render component, draw using geometry shader.
        // This requires the render system to provide a list of meshes/materials.

        // Set render targets: G‑buffer targets 0,1,2 and depth target.
        // This is device‑specific; we need to implement MRT in the backend.
        // For simplicity, we'll assume the device supports multiple render targets.
        IRenderDevice* device = m_renderer->GetDevice();
        if (!device) return;

        // Bind G‑buffer as render targets
        // (This would call device->SetRenderTargets(3, m_gBuffer, m_depthTarget);)
        // For now, we just bind each individually (pseudocode).
        device->SetRenderTarget(0, m_gBuffer[0]);
        device->SetRenderTarget(1, m_gBuffer[1]);
        device->SetRenderTarget(2, m_gBuffer[2]);
        device->SetDepthStencilTarget(m_depthTarget);

        device->Clear(CLEAR_COLOR | CLEAR_DEPTH, Color::Black, 1.0f, 0);

        // Bind geometry shader
        m_geometryShader->Bind();

        // Iterate over opaque entities, set modelViewProj uniform, draw mesh
        // (This part is application‑specific; we'll assume a global World instance)
        // For demonstration, we just log.
        USE_LOG_DEBUG("DeferredRenderer: Geometry pass completed.");
    }

    void DeferredRenderer::LightingPass(RenderTarget* destination)
    {
        if (!m_enabled || !destination) return;

        IRenderDevice* device = m_renderer->GetDevice();
        if (!device) return;

        // Set destination as render target (or backbuffer)
        device->SetRenderTarget(destination);
        device->SetDepthStencilTarget(nullptr); // lighting pass doesn't need depth

        // Bind lighting shader
        m_lightingShader->Bind();

        // Bind G‑buffer textures
        m_lightingShader->SetTexture("albedoSpec", m_gBuffer[0]->GetColorTexture());
        m_lightingShader->SetTexture("normalMetal", m_gBuffer[1]->GetColorTexture());

        // Set light properties (example: one directional light)
        Vector3 lightDir(0.0f, -1.0f, 0.0f);
        Vector3 lightColor(1.0f, 1.0f, 1.0f);
        float ambient = 0.2f;

        m_lightingShader->SetUniform("lightDir", lightDir);
        m_lightingShader->SetUniform("lightColor", lightColor);
        m_lightingShader->SetUniform("ambient", ambient);

        // Draw fullscreen quad
        if (m_fullscreenQuad) {
            m_fullscreenQuad->Bind(device);
            m_fullscreenQuad->Draw(device);
        }
    }

    void DeferredRenderer::RenderTransparent()
    {
        // Forward rendering pass for transparent objects (not part of deferred).
        // This would be implemented separately using the forward renderer.
    }

    bool DeferredRenderer::CreateShaders()
    {
        m_geometryShader = Shader::Create();
        if (!m_geometryShader->LoadFromSource(s_geometryVertex, s_geometryFragment)) {
            USE_LOG_ERROR("DeferredRenderer: Failed to compile geometry shader.");
            return false;
        }
        m_lightingShader = Shader::Create();
        if (!m_lightingShader->LoadFromSource(s_lightingVertex, s_lightingFragment)) {
            USE_LOG_ERROR("DeferredRenderer: Failed to compile lighting shader.");
            return false;
        }
        return true;
    }

    void DeferredRenderer::CreateFullscreenQuad()
    {
        if (!m_renderer) return;
        float vertices[] = {
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f
        };
        uint32_t indices[] = {0, 1, 2, 2, 1, 3};
        m_fullscreenQuad = new Mesh();
        m_fullscreenQuad->Create(m_renderer->GetDevice(), vertices, 4, 5 * sizeof(float), indices, 6);
    }

    // Helper to apply a single light (extend for point/spot)
    void DeferredRenderer::ApplyLight(Light* light, const Matrix4& viewProj)
    {
        // This would be called in a loop over all lights
        // We'd need to compute light bounds, stencil, etc.
    }

} // namespace USE