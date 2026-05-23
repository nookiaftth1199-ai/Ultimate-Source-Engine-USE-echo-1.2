// ============================================================
// Ultimate Source Engine - OpenGL Render Device Implementation
// ============================================================

#include "stdafx.h"
#include "GLDevice.h"
#include "Core/Window.h"
#include "Core/Logger.h"

#ifdef _WIN32
    #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

namespace USE {

    GLDevice::GLDevice()
        : m_context(nullptr)
        , m_window(nullptr)
        , m_initialized(false)
    {
        USE_LOG_INFO("GLDevice created.");
    }

    GLDevice::~GLDevice()
    {
        Shutdown();
    }

    bool GLDevice::Initialize(Window* window, bool vsync)
    {
        if (m_initialized) {
            USE_LOG_WARN("GLDevice already initialized.");
            return true;
        }

        USE_LOG_INFO("Initializing OpenGL device...");

        // Get SDL window handle (assuming Window is SDLWindow)
        // We need a way to get the native SDL_Window*; let's assume Window has a GetSDLWindow() method.
        // If not, we need to cast. For simplicity, we'll add a virtual method GetNativeHandle() returning void*.
        m_window = reinterpret_cast<SDL_Window*>(window->GetNativeHandle());
        if (!m_window) {
            USE_LOG_ERROR("Failed to get native window handle.");
            return false;
        }

        // Create OpenGL context
        m_context = SDL_GL_CreateContext(m_window);
        if (!m_context) {
            USE_LOG_ERROR("SDL_GL_CreateContext failed: %s", SDL_GetError());
            return false;
        }

        // Set VSync
        if (vsync) {
            if (SDL_GL_SetSwapInterval(1) != 0) {
                USE_LOG_WARN("VSync not supported: %s", SDL_GetError());
            }
        } else {
            SDL_GL_SetSwapInterval(0);
        }

        // Print OpenGL info
        USE_LOG_INFO("OpenGL Vendor: %s", glGetString(GL_VENDOR));
        USE_LOG_INFO("OpenGL Renderer: %s", glGetString(GL_RENDERER));
        USE_LOG_INFO("OpenGL Version: %s", glGetString(GL_VERSION));

        // Initialize basic OpenGL state
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        CheckGLError("Initialize");

        m_initialized = true;
        USE_LOG_INFO("OpenGL device initialized successfully.");
        return true;
    }

    void GLDevice::Shutdown()
    {
        if (!m_initialized) return;

        USE_LOG_INFO("Shutting down OpenGL device...");

        if (m_context) {
            SDL_GL_DeleteContext(m_context);
            m_context = nullptr;
        }

        m_initialized = false;
        USE_LOG_INFO("OpenGL device shut down.");
    }

    void GLDevice::BeginFrame()
    {
        // Nothing special for OpenGL
    }

    void GLDevice::EndFrame()
    {
        // Nothing special
    }

    void GLDevice::Present()
    {
        if (m_window) {
            SDL_GL_SwapWindow(m_window);
        }
    }

    void GLDevice::Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil)
    {
        GLbitfield glFlags = 0;
        if (flags & CLEAR_COLOR) {
            glClearColor(color.r, color.g, color.b, color.a);
            glFlags |= GL_COLOR_BUFFER_BIT;
        }
        if (flags & CLEAR_DEPTH) {
            glClearDepth(depth);
            glFlags |= GL_DEPTH_BUFFER_BIT;
        }
        if (flags & CLEAR_STENCIL) {
            glClearStencil(stencil);
            glFlags |= GL_STENCIL_BUFFER_BIT;
        }
        glClear(glFlags);
        CheckGLError("Clear");
    }

    void GLDevice::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
        CheckGLError("SetViewport");
    }

    void GLDevice::SetScissorRect(int x, int y, int width, int height)
    {
        glScissor(x, y, width, height);
        CheckGLError("SetScissorRect");
    }

    void GLDevice::EnableScissor(bool enable)
    {
        if (enable)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    void GLDevice::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
    {
        // This is a simplified implementation. In a real engine, you'd have vertex/index buffers bound.
        // For now, we just draw with immediate mode or assume buffers are bound.
        // We'll need to have the vertex/index buffer handles stored and bound before this call.
        // Since we don't have that, we'll just issue a placeholder.
        // In practice, you'd call glDrawElements with bound buffers.

        // For demonstration, we'll just draw a simple triangle with immediate mode.
        // But that's not correct for indexed drawing with buffers. We'll assume buffers are bound.
        // We'll use glDrawElements, which requires GL_ELEMENT_ARRAY_BUFFER bound.
        // We'll need to track currently bound index buffer.
        // To keep it simple, we'll just call glDrawElements with a dummy call.
        // This will cause an error if no buffer is bound. So we'll check.

        // For OpenGL 2.0, we can use glDrawElements with client-side vertex arrays or VBOs.
        // We'll assume VBOs are used and bound.

        // For now, we'll just log an error if no buffers are bound.
        USE_LOG_WARN("DrawIndexed called but not fully implemented.");
    }

    bool GLDevice::CreateVertexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        bufferHandle = static_cast<uint32_t>(vbo);
        CheckGLError("CreateVertexBuffer");
        return true;
    }

    bool GLDevice::CreateIndexBuffer(const void* data, size_t size, uint32_t& bufferHandle)
    {
        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        bufferHandle = static_cast<uint32_t>(ibo);
        CheckGLError("CreateIndexBuffer");
        return true;
    }

    bool GLDevice::CreateTexture2D(int width, int height, TextureFormat format, const void* data, uint32_t& textureHandle)
    {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Convert TextureFormat to OpenGL internal format and pixel format
        GLint internalFormat = GL_RGBA;
        GLenum pixelFormat = GL_RGBA;
        GLenum pixelType = GL_UNSIGNED_BYTE;

        // For simplicity, assume RGBA8 for now. We'll add a switch later.
        // TODO: map TextureFormat to OpenGL constants.

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                     pixelFormat, pixelType, data);

        // Set default texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        textureHandle = static_cast<uint32_t>(tex);
        CheckGLError("CreateTexture2D");
        return true;
    }

    bool GLDevice::CreateShader(ShaderType type, const char* source, uint32_t& shaderHandle)
    {
        GLenum glShaderType;
        switch (type) {
            case ShaderType::Vertex:   glShaderType = GL_VERTEX_SHADER; break;
            case ShaderType::Pixel:    glShaderType = GL_FRAGMENT_SHADER; break;
            default:
                USE_LOG_ERROR("Unsupported shader type for OpenGL 2.0");
                return false;
        }

        GLuint shader = glCreateShader(glShaderType);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Check compile status
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = new char[infoLen];
                glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
                USE_LOG_ERROR("Shader compile error: %s", infoLog);
                delete[] infoLog;
            }
            glDeleteShader(shader);
            return false;
        }

        shaderHandle = static_cast<uint32_t>(shader);
        CheckGLError("CreateShader");
        return true;
    }

    void GLDevice::DestroyBuffer(uint32_t handle)
    {
        GLuint buffer = static_cast<GLuint>(handle);
        glDeleteBuffers(1, &buffer);
    }

    void GLDevice::DestroyTexture(uint32_t handle)
    {
        GLuint tex = static_cast<GLuint>(handle);
        glDeleteTextures(1, &tex);
    }

    void GLDevice::DestroyShader(uint32_t handle)
    {
        GLuint shader = static_cast<GLuint>(handle);
        glDeleteShader(shader);
    }

    const char* GLDevice::GetVendor() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    }

    const char* GLDevice::GetRenderer() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }

    const char* GLDevice::GetVersion() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VERSION));
    }

    void GLDevice::CheckGLError(const char* context)
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            const char* errStr = "Unknown error";
            switch (err) {
                case GL_INVALID_ENUM: errStr = "GL_INVALID_ENUM"; break;
                case GL_INVALID_VALUE: errStr = "GL_INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: errStr = "GL_INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW: errStr = "GL_STACK_OVERFLOW"; break;
                case GL_STACK_UNDERFLOW: errStr = "GL_STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY: errStr = "GL_OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: errStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            USE_LOG_WARN("OpenGL error in %s: %s (0x%04X)", context, errStr, err);
        }
    }

} // namespace USE