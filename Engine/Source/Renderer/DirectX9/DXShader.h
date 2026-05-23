// ============================================================
// Ultimate Source Engine - DirectX 9 Shader
// ============================================================
//
// Represents a vertex or pixel shader for DirectX 9.
// Supports compilation from source or file, and setting constants.
// ============================================================

#pragma once

#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace USE {

    enum class ShaderType {
        Vertex,
        Pixel
    };

    class DXShader {
    public:
        DXShader();
        ~DXShader();

        // Load and compile from source string
        bool LoadFromSource(ShaderType type, const char* source,
                            const char* entryPoint = "main",
                            const char* profile = nullptr);

        // Load from file
        bool LoadFromFile(ShaderType type, const char* filename,
                          const char* entryPoint = "main",
                          const char* profile = nullptr);

        // Bind/unbind the shader for rendering
        void Bind(IDirect3DDevice9* device);
        void Unbind(IDirect3DDevice9* device);

        // Check if shader is valid
        bool IsValid() const { return m_shader != nullptr; }

        // Get DirectX handle
        IDirect3DVertexShader9* GetVertexShader() const {
            return (m_type == ShaderType::Vertex) ? (IDirect3DVertexShader9*)m_shader : nullptr;
        }
        IDirect3DPixelShader9* GetPixelShader() const {
            return (m_type == ShaderType::Pixel) ? (IDirect3DPixelShader9*)m_shader : nullptr;
        }

        // Set constants (using constant table)
        void SetBool(const char* name, bool value);
        void SetInt(const char* name, int value);
        void SetFloat(const char* name, float value);
        void SetVector(const char* name, const float* values, int count = 4);
        void SetMatrix(const char* name, const float* matrix);

        // Set constants by register index (faster, no name lookup)
        void SetBool(int reg, bool value);
        void SetInt(int reg, int value);
        void SetFloat(int reg, float value);
        void SetVector(int reg, const float* values, int count = 4);
        void SetMatrix(int reg, const float* matrix);

    private:
        ShaderType            m_type;
        IUnknown*             m_shader;         // IDirect3DVertexShader9* or IDirect3DPixelShader9*
        LPD3DXCONSTANTTABLE   m_constTable;     // For setting constants by name
        IDirect3DDevice9*     m_device;         // Device used for constant setting (needed for Set* calls)

        void Release();
        bool Compile(ShaderType type, const char* source, const char* entryPoint, const char* profile);
    };

} // namespace USE