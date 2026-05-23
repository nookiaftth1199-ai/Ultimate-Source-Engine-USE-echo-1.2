// ============================================================
// Ultimate Source Engine - OpenGL Shader Program Implementation
// ============================================================

#include "stdafx.h"
#include "GLProgram.h"
#include "Core/Logger.h"

#include <GL/gl.h>
#include <GL/glu.h>

namespace USE {

    GLProgram::GLProgram()
        : m_program(0)
    {
    }

    GLProgram::~GLProgram()
    {
        if (m_program) {
            // Detach any attached shaders (optional, they will be deleted separately)
            // Actually, shaders should be detached before program deletion if they are to be deleted.
            // But we can leave it to the user to manage shader lifetimes.
            glDeleteProgram(m_program);
        }
    }

    void GLProgram::AttachShader(unsigned int shaderHandle)
    {
        if (!m_program) {
            m_program = glCreateProgram();
            if (!m_program) {
                USE_LOG_ERROR("GLProgram: failed to create program object");
                return;
            }
        }

        glAttachShader(m_program, shaderHandle);
        m_attachedShaders.push_back(shaderHandle);
    }

    bool GLProgram::Link()
    {
        if (!m_program) {
            USE_LOG_ERROR("GLProgram::Link: no program created");
            return false;
        }

        glLinkProgram(m_program);

        // Check link status
        GLint linked;
        glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint infoLen = 0;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = new char[infoLen];
                glGetProgramInfoLog(m_program, infoLen, nullptr, infoLog);
                USE_LOG_ERROR("GLProgram link error:\n%s", infoLog);
                delete[] infoLog;
            }
            return false;
        }

        USE_LOG_DEBUG("GLProgram linked successfully");
        return true;
    }

    void GLProgram::Use()
    {
        if (m_program) {
            glUseProgram(m_program);
        }
    }

    int GLProgram::GetUniformLocation(const char* name)
    {
        return glGetUniformLocation(m_program, name);
    }

    void GLProgram::SetUniform(const char* name, int value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1i(loc, value);
    }

    void GLProgram::SetUniform(const char* name, float value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1f(loc, value);
    }

    void GLProgram::SetUniform(const char* name, const Vector2& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform2f(loc, value.x, value.y);
    }

    void GLProgram::SetUniform(const char* name, const Vector3& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform3f(loc, value.x, value.y, value.z);
    }

    void GLProgram::SetUniform(const char* name, const Vector4& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform4f(loc, value.x, value.y, value.z, value.w);
    }

    void GLProgram::SetUniform(const char* name, const Color& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform4f(loc, value.r, value.g, value.b, value.a);
    }

    void GLProgram::SetUniform(const char* name, const Matrix4& value, bool transpose)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, &value.m[0][0]);
        }
    }

    void GLProgram::SetUniform(const char* name, int count, const int* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1iv(loc, count, values);
    }

    void GLProgram::SetUniform(const char* name, int count, const float* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1fv(loc, count, values);
    }

    void GLProgram::SetUniform(const char* name, int count, const Vector2* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform2fv(loc, count, &values[0].x);
    }

    void GLProgram::SetUniform(const char* name, int count, const Vector3* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform3fv(loc, count, &values[0].x);
    }

    void GLProgram::SetUniform(const char* name, int count, const Vector4* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform4fv(loc, count, &values[0].x);
    }

} // namespace USE