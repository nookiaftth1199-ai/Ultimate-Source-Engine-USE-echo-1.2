// ============================================================
// Ultimate Source Engine - OpenGL Shader Program
// ============================================================
//
// Represents a linked OpenGL shader program.
// Manages attaching shaders, linking, and uniform setting.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include <vector>

namespace USE {

    class GLProgram {
    public:
        GLProgram();
        ~GLProgram();

        // Attach a compiled shader (vertex, fragment, geometry, etc.)
        void AttachShader(unsigned int shaderHandle);

        // Link the program; returns true on success.
        bool Link();

        // Use the program for rendering
        void Use();

        // Check if program is linked and valid
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

        // Uniform array setters
        void SetUniform(const char* name, int count, const int* values);
        void SetUniform(const char* name, int count, const float* values);
        void SetUniform(const char* name, int count, const Vector2* values);
        void SetUniform(const char* name, int count, const Vector3* values);
        void SetUniform(const char* name, int count, const Vector4* values);

    private:
        unsigned int m_program;
        std::vector<unsigned int> m_attachedShaders; // keep track for potential cleanup (optional)

        int GetUniformLocation(const char* name);
    };

} // namespace USE