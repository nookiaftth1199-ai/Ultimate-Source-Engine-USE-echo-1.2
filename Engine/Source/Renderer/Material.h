// ============================================================
// Ultimate Source Engine - Material
// ============================================================
//
// Material system that holds shader, textures, uniforms, and render states.
// Supports PBR properties (metallic, roughness, occlusion, emissive).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/RenderTypes.h"
#include <map>
#include <string>

namespace USE {

    // Material class
    class Material {
    public:
        Material(const char* name = "Unnamed");
        ~Material();

        // Set/get the shader
        void SetShader(Shader* shader);
        Shader* GetShader() const { return m_shader; }

        // Texture slots (standardised mapping)
        void SetTexture(int slot, Texture* texture);
        Texture* GetTexture(int slot) const;
        void ClearTexture(int slot);

        // Uniform setters (stored and applied on bind)
        void SetUniform(const char* name, int value);
        void SetUniform(const char* name, float value);
        void SetUniform(const char* name, const Vector2& value);
        void SetUniform(const char* name, const Vector3& value);
        void SetUniform(const char* name, const Vector4& value);
        void SetUniform(const char* name, const Color& value);
        void SetUniform(const char* name, const Matrix4& value, bool transpose = false);

        // PBR convenience setters
        void SetAlbedo(const Color& color) { SetUniform("material.albedo", color); }
        void SetMetallic(float value)      { SetUniform("material.metallic", value); }
        void SetRoughness(float value)     { SetUniform("material.roughness", value); }
        void SetAmbientOcclusion(float value) { SetUniform("material.ao", value); }
        void SetEmissive(const Color& color)  { SetUniform("material.emissive", color); }
        void SetEmissiveStrength(float strength) { SetUniform("material.emissiveStrength", strength); }

        // Texture assignment shortcuts
        void SetAlbedoMap(Texture* tex)  { SetTexture(0, tex); }
        void SetNormalMap(Texture* tex)  { SetTexture(1, tex); }
        void SetMetallicMap(Texture* tex){ SetTexture(2, tex); }
        void SetRoughnessMap(Texture* tex){ SetTexture(3, tex); }
        void SetAOMap(Texture* tex)      { SetTexture(4, tex); }
        void SetEmissiveMap(Texture* tex){ SetTexture(5, tex); }

        // Remove a uniform
        void RemoveUniform(const char* name);

        // Render state properties
        void SetBlendMode(BlendMode mode)          { m_blendMode = mode; }
        void SetDepthTest(bool enable)              { m_depthTest = enable; }
        void SetDepthWrite(bool enable)             { m_depthWrite = enable; }
        void SetCullMode(CullMode mode)             { m_cullMode = mode; }
        void SetFillMode(FillMode mode)             { m_fillMode = mode; }
        void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f);

        BlendMode GetBlendMode() const { return m_blendMode; }
        bool      GetDepthTest() const { return m_depthTest; }
        bool      GetDepthWrite() const { return m_depthWrite; }
        CullMode  GetCullMode() const { return m_cullMode; }
        FillMode  GetFillMode() const { return m_fillMode; }

        // Bind the material (applies shader, textures, uniforms, render states)
        void Bind();

        // Unbind (optional, resets states if needed)
        void Unbind();

        // Name (for debugging)
        const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;
        Shader*     m_shader;

        static const int MAX_TEXTURE_SLOTS = 16;
        Texture* m_textures[MAX_TEXTURE_SLOTS];

        // Uniform storage
        std::map<std::string, int>        m_intUniforms;
        std::map<std::string, float>      m_floatUniforms;
        std::map<std::string, Vector2>    m_vec2Uniforms;
        std::map<std::string, Vector3>    m_vec3Uniforms;
        std::map<std::string, Vector4>    m_vec4Uniforms;
        std::map<std::string, Matrix4>    m_mat4Uniforms;
        std::map<std::string, bool>       m_boolUniforms;

        // Render states
        BlendMode m_blendMode;
        bool      m_depthTest;
        bool      m_depthWrite;
        CullMode  m_cullMode;
        FillMode  m_fillMode;
        bool      m_polygonOffsetEnable;
        float     m_polygonOffsetFactor;
        float     m_polygonOffsetUnits;

        // Helper to apply render states (backend‑agnostic; uses immediate OpenGL for now)
        void ApplyRenderStates();
    };

} // namespace USE