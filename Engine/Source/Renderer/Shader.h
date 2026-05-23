// ============================================================
// Ultimate Source Engine - Shader Interface
// ============================================================
//
// Abstract base class for shaders. Provides a common interface
// for loading, binding, and setting uniforms across all
// rendering backends.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"

namespace USE {

    // Shader type (vertex, fragment, geometry)
    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        Compute
    };

    // Shader interface
    class Shader {
    public:
        virtual ~Shader() {}

        // Load from file (source or pre‑compiled)
        virtual bool LoadFromFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) = 0;

        // Load from source strings
        virtual bool LoadFromSource(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr) = 0;

        // Bind/unbind the shader for rendering
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        // Check if shader is valid
        virtual bool IsValid() const = 0;

        // Uniform setters (by name)
        virtual void SetUniform(const char* name, int value) = 0;
        virtual void SetUniform(const char* name, float value) = 0;
        virtual void SetUniform(const char* name, const Vector2& value) = 0;
        virtual void SetUniform(const char* name, const Vector3& value) = 0;
        virtual void SetUniform(const char* name, const Vector4& value) = 0;
        virtual void SetUniform(const char* name, const Color& value) = 0;
        virtual void SetUniform(const char* name, const Matrix4& value, bool transpose = false) = 0;

        // Uniform array setters
        virtual void SetUniform(const char* name, int count, const int* values) = 0;
        virtual void SetUniform(const char* name, int count, const float* values) = 0;
        virtual void SetUniform(const char* name, int count, const Vector2* values) = 0;
        virtual void SetUniform(const char* name, int count, const Vector3* values) = 0;
        virtual void SetUniform(const char* name, int count, const Vector4* values) = 0;

        // Static factory method: creates a shader for the current render backend
        static Shader* Create();
    };

} // namespace USE