// ============================================================
// Ultimate Source Engine - Skybox Implementation
// ============================================================

#include "stdafx.h"
#include "Skybox.h"
#include "Core/Engine.h"
#include "Renderer/RenderSystem.h"
#include "Renderer/Camera.h"
#include "Resources/ResourceManager.h"
#include "Core/Logger.h"

namespace USE {

    // Default cubemap shader source (GLSL 1.20 – OpenGL 2.1)
    static const char* s_skyboxVertexSource =
        "varying vec3 vUVW;\n"
        "void main() {\n"
        "    vUVW = position.xyz;\n"  // position from vertex (cube centered at origin)
        "    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);\n"
        "}\n";

    static const char* s_skyboxFragmentSource =
        "uniform samplerCube skybox;\n"
        "varying vec3 vUVW;\n"
        "void main() {\n"
        "    gl_FragColor = textureCube(skybox, vUVW);\n"
        "}\n";

    Skybox::Skybox()
        : m_cubeMesh(nullptr)
        , m_material(nullptr)
    {
    }

    Skybox::~Skybox()
    {
        if (m_cubeMesh) {
            delete m_cubeMesh;
            m_cubeMesh = nullptr;
        }
        // Material is owned by resource manager or external; we don't delete it here.
    }

    bool Skybox::LoadFromFiles(const char* right, const char* left,
                               const char* top, const char* bottom,
                               const char* front, const char* back)
    {
        // Create cube mesh if not already created
        if (!m_cubeMesh) {
            CreateCubeMesh();
            if (!m_cubeMesh || !m_cubeMesh->IsValid()) {
                USE_LOG_ERROR("Skybox: Failed to create cube mesh");
                return false;
            }
        }

        // Create material with cubemap shader
        // In a real engine, you would use the resource manager to load/create shaders.
        // For simplicity, we'll create a Shader from source.
        Shader* shader = Shader::Create();
        if (!shader || !shader->LoadFromSource(s_skyboxVertexSource, s_skyboxFragmentSource)) {
            USE_LOG_ERROR("Skybox: Failed to create cubemap shader");
            delete shader;
            return false;
        }

        // Create material
        Material* mat = new Material("Skybox");
        mat->SetShader(shader);

        // Set up texture (cubemap) – we need to create a cubemap texture.
        // The engine does not have a cubemap texture class yet, so we'll use the render device directly.
        // This is device‑specific; we'll use OpenGL for now, but ideally we'd extend IRenderDevice.
        // We'll leave a TODO: implement cubemap creation via device.
        #ifdef USE_OPENGL
        // OpenGL specific code to create cubemap from six files.
        // This is just a placeholder; you'll need to implement actual texture loading.
        // For now, we'll just set a null texture and warn.
        USE_LOG_WARN("Skybox: Cubemap texture creation not implemented.");
        #endif

        // For now, we'll skip texture creation; the material will have no texture.
        m_material = mat;

        USE_LOG_INFO("Skybox loaded (cubemap texture not yet created)");
        return true;
    }

    bool Skybox::LoadFromCubemap(const char* filename)
    {
        // Not implemented
        USE_LOG_ERROR("Skybox::LoadFromCubemap not implemented");
        return false;
    }

    void Skybox::Render(Camera* camera)
    {
        if (!m_cubeMesh || !m_material || !m_material->GetShader()) {
            USE_LOG_WARN("Skybox: Not ready for rendering");
            return;
        }

        // Disable depth writes, but keep depth test (so skybox doesn't overwrite)
        // We'll set render states via material or directly via render device.
        // For now, we'll use OpenGL immediate state changes (bad, but as placeholder).
        #ifdef USE_OPENGL
        glDepthMask(GL_FALSE);
        #endif

        // Get view matrix without translation (remove the translation part)
        Matrix4 view = camera->GetViewMatrix();
        view.m[3][0] = 0.0f;
        view.m[3][1] = 0.0f;
        view.m[3][2] = 0.0f;

        // Get projection matrix
        Matrix4 proj = camera->GetProjectionMatrix();

        // Set uniforms in material
        m_material->SetUniform("viewMatrix", view);
        m_material->SetUniform("projectionMatrix", proj);
        // The vertex shader expects "position" attribute – our mesh must provide that.

        // Bind material
        m_material->Bind();

        // Draw mesh
        RenderSystem* renderer = Engine::Get()->GetRenderer();
        if (renderer) {
            m_cubeMesh->Bind(renderer->GetDevice());
            m_cubeMesh->Draw(renderer->GetDevice());
        }

        // Restore depth mask
        #ifdef USE_OPENGL
        glDepthMask(GL_TRUE);
        #endif
    }

    void Skybox::CreateCubeMesh()
    {
        // Vertices of a unit cube (12 triangles, 36 vertices)
        float vertices[] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        uint32_t indexCount = 36; // no index buffer needed; we use vertices directly as triangles.

        RenderSystem* renderer = Engine::Get()->GetRenderer();
        if (!renderer) return;

        IRenderDevice* device = renderer->GetDevice();

        m_cubeMesh = new Mesh();
        bool success = m_cubeMesh->Create(device, vertices, 36, 3 * sizeof(float), nullptr, 0);
        if (!success) {
            USE_LOG_ERROR("Skybox: Failed to create cube mesh");
            delete m_cubeMesh;
            m_cubeMesh = nullptr;
        }
    }

} // namespace USE