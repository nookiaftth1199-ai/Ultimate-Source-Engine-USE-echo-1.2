// ============================================================
// Ultimate Source Engine - OpenGL Shader Implementation
// ============================================================

#include "stdafx.h"
#include "GLShader.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Utility/StringUtils.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include <map>

namespace USE {

    GLShader::GLShader()
        : m_vertexShader(0)
        , m_fragmentShader(0)
        , m_program(0)
    {
    }

    GLShader::~GLShader()
    {
        if (m_program) {
            glDeleteProgram(m_program);
        }
        if (m_vertexShader) {
            glDeleteShader(m_vertexShader);
        }
        if (m_fragmentShader) {
            glDeleteShader(m_fragmentShader);
        }
    }

    bool GLShader::LoadFromSource(const char* vertexSource, const char* fragmentSource)
    {
        if (!vertexSource || !fragmentSource) {
            USE_LOG_ERROR("GLShader::LoadFromSource: null source");
            return false;
        }

        // Compile vertex shader
        if (!CompileShader(m_vertexShader, GL_VERTEX_SHADER, vertexSource)) {
            USE_LOG_ERROR("Failed to compile vertex shader");
            return false;
        }

        // Compile fragment shader
        if (!CompileShader(m_fragmentShader, GL_FRAGMENT_SHADER, fragmentSource)) {
            USE_LOG_ERROR("Failed to compile fragment shader");
            return false;
        }

        // Link program
        if (!LinkProgram()) {
            USE_LOG_ERROR("Failed to link shader program");
            return false;
        }

        USE_LOG_INFO("Shader loaded successfully");
        return true;
    }

    bool GLShader::LoadFromFile(const char* vertexPath, const char* fragmentPath)
    {
        // Use FileSystem to read files
        FileSystem* fs = FileSystem::Get(); // We'll need to access the file system; assume singleton or global.
        if (!fs) {
            USE_LOG_ERROR("GLShader::LoadFromFile: FileSystem not available");
            return false;
        }

        std::string vertexSource;
        std::string fragmentSource;

        auto vFile = fs->OpenFile(vertexPath, FILE_READ | FILE_TEXT);
        if (!vFile) {
            USE_LOG_ERROR("Failed to open vertex shader file: %s", vertexPath);
            return false;
        }
        vFile->ReadAll(vertexSource);
        vFile->Close();

        auto fFile = fs->OpenFile(fragmentPath, FILE_READ | FILE_TEXT);
        if (!fFile) {
            USE_LOG_ERROR("Failed to open fragment shader file: %s", fragmentPath);
            return false;
        }
        fFile->ReadAll(fragmentSource);
        fFile->Close();

        return LoadFromSource(vertexSource.c_str(), fragmentSource.c_str());
    }

    void GLShader::Bind()
    {
        if (m_program) {
            glUseProgram(m_program);
        }
    }

    void GLShader::Unbind()
    {
        glUseProgram(0);
    }

    bool GLShader::CompileShader(unsigned int& shader, unsigned int type, const char* source)
    {
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Check compilation status
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = new char[infoLen];
                glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
                USE_LOG_ERROR("Shader compilation error:\n%s", infoLog);
                delete[] infoLog;
            }
            glDeleteShader(shader);
            shader = 0;
            return false;
        }

        return true;
    }

    bool GLShader::LinkProgram()
    {
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertexShader);
        glAttachShader(m_program, m_fragmentShader);
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
                USE_LOG_ERROR("Shader link error:\n%s", infoLog);
                delete[] infoLog;
            }
            glDeleteProgram(m_program);
            m_program = 0;
            return false;
        }

        return true;
    }

    int GLShader::GetUniformLocation(const char* name)
    {
        // For simplicity, query each time. Could cache in a map.
        return glGetUniformLocation(m_program, name);
    }

    void GLShader::SetUniform(const char* name, int value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1i(loc, value);
    }

    void GLShader::SetUniform(const char* name, float value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1f(loc, value);
    }

    void GLShader::SetUniform(const char* name, const Vector2& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform2f(loc, value.x, value.y);
    }

    void GLShader::SetUniform(const char* name, const Vector3& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform3f(loc, value.x, value.y, value.z);
    }

    void GLShader::SetUniform(const char* name, const Vector4& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform4f(loc, value.x, value.y, value.z, value.w);
    }

    void GLShader::SetUniform(const char* name, const Color& value)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform4f(loc, value.r, value.g, value.b, value.a);
    }

    void GLShader::SetUniform(const char* name, const Matrix4& value, bool transpose)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) {
            // OpenGL expects column-major matrices. Our Matrix4 is column-major already.
            glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, &value.m[0][0]);
        }
    }

    void GLShader::SetUniform(const char* name, int count, const int* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1iv(loc, count, values);
    }

    void GLShader::SetUniform(const char* name, int count, const float* values)
    {
        int loc = GetUniformLocation(name);
        if (loc != -1) glUniform1fv(loc, count, values);
    }

} // namespace USE