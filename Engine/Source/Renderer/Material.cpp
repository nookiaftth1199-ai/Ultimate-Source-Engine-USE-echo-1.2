// ============================================================
// Ultimate Source Engine - Material Implementation
// ============================================================

#include "stdafx.h"
#include "Material.h"
#include "Core/Engine.h"
#include "Renderer/RenderSystem.h"
#include "Core/Logger.h"

// For render state setting (temporary; should go through IRenderDevice)
#include <GL/gl.h>

namespace USE {

    Material::Material(const char* name)
        : m_name(name)
        , m_shader(nullptr)
        , m_blendMode(BlendMode::Opaque)
        , m_depthTest(true)
        , m_depthWrite(true)
        , m_cullMode(CullMode::Back)
        , m_fillMode(FillMode::Solid)
        , m_polygonOffsetEnable(false)
        , m_polygonOffsetFactor(0.0f)
        , m_polygonOffsetUnits(0.0f)
    {
        for (int i = 0; i < MAX_TEXTURE_SLOTS; ++i) {
            m_textures[i] = nullptr;
        }
    }

    Material::~Material()
    {
        // Textures and shader are owned by ResourceManager; we don't delete them.
    }

    void Material::SetShader(Shader* shader)
    {
        m_shader = shader;
    }

    void Material::SetTexture(int slot, Texture* texture)
    {
        if (slot >= 0 && slot < MAX_TEXTURE_SLOTS) {
            m_textures[slot] = texture;
        } else {
            USE_LOG_ERROR("Material::SetTexture: slot %d out of range", slot);
        }
    }

    Texture* Material::GetTexture(int slot) const
    {
        if (slot >= 0 && slot < MAX_TEXTURE_SLOTS) {
            return m_textures[slot];
        }
        return nullptr;
    }

    void Material::ClearTexture(int slot)
    {
        if (slot >= 0 && slot < MAX_TEXTURE_SLOTS) {
            m_textures[slot] = nullptr;
        }
    }

    void Material::SetUniform(const char* name, int value)
    {
        m_intUniforms[name] = value;
    }

    void Material::SetUniform(const char* name, float value)
    {
        m_floatUniforms[name] = value;
    }

    void Material::SetUniform(const char* name, const Vector2& value)
    {
        m_vec2Uniforms[name] = value;
    }

    void Material::SetUniform(const char* name, const Vector3& value)
    {
        m_vec3Uniforms[name] = value;
    }

    void Material::SetUniform(const char* name, const Vector4& value)
    {
        m_vec4Uniforms[name] = value;
    }

    void Material::SetUniform(const char* name, const Color& value)
    {
        // Color is essentially Vector4
        m_vec4Uniforms[name] = Vector4(value.r, value.g, value.b, value.a);
    }

    void Material::SetUniform(const char* name, const Matrix4& value, bool transpose)
    {
        m_mat4Uniforms[name] = value;
        // transpose flag is not stored; we'll pass false to shader because our matrices are column‑major.
    }

    void Material::RemoveUniform(const char* name)
    {
        m_intUniforms.erase(name);
        m_floatUniforms.erase(name);
        m_vec2Uniforms.erase(name);
        m_vec3Uniforms.erase(name);
        m_vec4Uniforms.erase(name);
        m_mat4Uniforms.erase(name);
        m_boolUniforms.erase(name);
    }

    void Material::SetPolygonOffset(bool enable, float factor, float units)
    {
        m_polygonOffsetEnable = enable;
        m_polygonOffsetFactor = factor;
        m_polygonOffsetUnits = units;
    }

    void Material::Bind()
    {
        if (!m_shader || !m_shader->IsValid()) {
            USE_LOG_WARN("Material::Bind: no valid shader for material '%s'", m_name.c_str());
            return;
        }

        m_shader->Bind();

        // Bind textures
        for (int i = 0; i < MAX_TEXTURE_SLOTS; ++i) {
            if (m_textures[i] && m_textures[i]->IsValid()) {
                m_textures[i]->Bind(i);
            }
        }

        // Apply all stored uniforms
        for (auto& kv : m_intUniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second);
        }
        for (auto& kv : m_floatUniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second);
        }
        for (auto& kv : m_vec2Uniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second);
        }
        for (auto& kv : m_vec3Uniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second);
        }
        for (auto& kv : m_vec4Uniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second);
        }
        for (auto& kv : m_mat4Uniforms) {
            m_shader->SetUniform(kv.first.c_str(), kv.second, false);
        }

        // Apply render states
        ApplyRenderStates();
    }

    void Material::Unbind()
    {
        // Unbind textures (optional)
        for (int i = 0; i < MAX_TEXTURE_SLOTS; ++i) {
            if (m_textures[i] && m_textures[i]->IsValid()) {
                m_textures[i]->Unbind(i);
            }
        }
        if (m_shader) {
            m_shader->Unbind();
        }
    }

    void Material::ApplyRenderStates()
    {
        // Using immediate OpenGL for now; should be moved to IRenderDevice abstraction.
        // Depth test
        if (m_depthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        // Depth write
        glDepthMask(m_depthWrite ? GL_TRUE : GL_FALSE);

        // Cull mode
        switch (m_cullMode) {
            case CullMode::None:
                glDisable(GL_CULL_FACE);
                break;
            case CullMode::Front:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                break;
            case CullMode::Back:
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                break;
        }

        // Fill mode (polygon mode)
        glPolygonMode(GL_FRONT_AND_BACK, m_fillMode == FillMode::Wireframe ? GL_LINE : GL_FILL);

        // Polygon offset
        if (m_polygonOffsetEnable) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(m_polygonOffsetFactor, m_polygonOffsetUnits);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        // Blend mode
        switch (m_blendMode) {
            case BlendMode::Opaque:
                glDisable(GL_BLEND);
                break;
            case BlendMode::AlphaBlend:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquation(GL_FUNC_ADD);
                break;
            case BlendMode::Additive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glBlendEquation(GL_FUNC_ADD);
                break;
            case BlendMode::Multiply:
                glEnable(GL_BLEND);
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
                glBlendEquation(GL_FUNC_ADD);
                break;
        }
    }

} // namespace USE