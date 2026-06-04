// ============================================================
// Ultimate Source Engine - OpenGL Shader
// ============================================================
//
// Represents a combined vertex/fragment shader program.
// Supports compilation, linking, and uniform setting.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"

namespace USE {

    class GLShader {
    public:
        GLShader();
        ~GLShader();

        // Load and compile shader from source strings
        bool LoadFromSource(const char* vertexSource, const char* fragmentSource);

        // Load from files (convenience)
        bool LoadFromFile(const char* vertexPath, const char* fragmentPath);

        // Bind/unbind the shader for rendering
        void Bind();
        void Unbind();

        // Check if shader is valid
        bool IsValid() const { return m_program != 0; }

        // Get OpenGL program handle
        unsigned int GetHandle() const { return m_program; }

        // Uniform setters
        void SetUniform(const char* name, int value);
        void SetUniform(const char* name, float value);
        void SetUniform(const char* name, const Vector2& value);
        void SetUniform(const char* name, const Vector3& value);
        void SetUniform(const char* name, const Vector4& value);
        void SetUniform(const char* name, const Color& value);
        void SetUniform(const char* name, const Matrix4& value, bool transpose = false);

        // Uniform array setters (for future use)
        void SetUniform(const char* name, int count, const int* values);
        void SetUniform(const char* name, int count, const float* values);

    private:
        unsigned int m_vertexShader;
        unsigned int m_fragmentShader;
        unsigned int m_program;

        // Compile a single shader
        bool CompileShader(unsigned int& shader, unsigned int type, const char* source);

        // Link the program
        bool LinkProgram();

        // Get uniform location (cached? we could cache in a map)
        int GetUniformLocation(const char* name);
    };

} // namespace USE