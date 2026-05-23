// ============================================================
// Ultimate Source Engine - OpenGL Framebuffer Implementation
// ============================================================

#include "stdafx.h"
#include "GLFramebuffer.h"
#include "GLTexture.h"
#include "Core/Logger.h"

#include <GL/gl.h>
#include <GL/glext.h> // For framebuffer extensions (if needed)

// On Windows, glext.h provides the necessary constants and function pointers.
// For simplicity, we assume the system has the necessary OpenGL extensions.
// We'll use dynamic function loading or rely on the fact that OpenGL 3.0+ has FBOs core.
// Since we target OpenGL 2.0, we must use extensions. We'll define the function pointers here.

#ifndef GL_FRAMEBUFFER_EXT
    #define GL_FRAMEBUFFER_EXT 0x8D40
#endif
#ifndef GL_RENDERBUFFER_EXT
    #define GL_RENDERBUFFER_EXT 0x8D41
#endif
#ifndef GL_COLOR_ATTACHMENT0_EXT
    #define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#endif
#ifndef GL_DEPTH_ATTACHMENT_EXT
    #define GL_DEPTH_ATTACHMENT_EXT 0x8D00
#endif
#ifndef GL_STENCIL_ATTACHMENT_EXT
    #define GL_STENCIL_ATTACHMENT_EXT 0x8D20
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
    #define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
    #define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
    #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
#endif
#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
    #define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
#endif
#ifndef GL_FRAMEBUFFER_UNSUPPORTED_EXT
    #define GL_FRAMEBUFFER_UNSUPPORTED_EXT 0x8CDD
#endif

// Function pointers for FBO functions
static PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = nullptr;
static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = nullptr;
static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = nullptr;
static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = nullptr;
static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = nullptr;

namespace USE {

    // Helper to initialize FBO function pointers (call once)
    static bool InitFBOPointers() {
        static bool initialized = false;
        if (initialized) return true;
        // In a real engine, you'd use a proper extension loader (e.g., GLEW).
        // Here we assume the pointers are already set or we set them manually.
        // For simplicity, we'll just return false if not available.
        // For Windows, you might use wglGetProcAddress.
#ifdef _WIN32
        glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
        glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
        glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
        glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
        glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
#endif
        if (!glGenFramebuffersEXT || !glDeleteFramebuffersEXT || !glBindFramebufferEXT ||
            !glFramebufferTexture2DEXT || !glCheckFramebufferStatusEXT) {
            return false;
        }
        initialized = true;
        return true;
    }

    GLFramebuffer::GLFramebuffer()
        : m_fbo(0)
        , m_width(0)
        , m_height(0)
        , m_depthStencilAttachment(nullptr)
        , m_numColorAttachments(0)
    {
        for (int i = 0; i < 8; ++i) m_colorAttachments[i] = nullptr;
    }

    GLFramebuffer::~GLFramebuffer()
    {
        Destroy();
    }

    bool GLFramebuffer::Create(int width, int height, TextureFormat colorFormat,
                               bool createDepthStencil, TextureFormat depthFormat)
    {
        if (!InitFBOPointers()) {
            USE_LOG_ERROR("GLFramebuffer: FBO extensions not available");
            return false;
        }

        if (m_fbo) Destroy();

        m_width = width;
        m_height = height;

        glGenFramebuffersEXT(1, &m_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        // Create color texture (single for now)
        GLTexture* colorTex = new GLTexture();
        if (!colorTex->Create(width, height, colorFormat)) {
            USE_LOG_ERROR("GLFramebuffer: Failed to create color texture");
            delete colorTex;
            glDeleteFramebuffersEXT(1, &m_fbo);
            m_fbo = 0;
            return false;
        }
        m_colorAttachments[0] = colorTex;
        m_numColorAttachments = 1;

        // Attach color texture
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, colorTex->GetHandle(), 0);

        // Create depth/stencil texture if requested
        if (createDepthStencil) {
            GLTexture* depthTex = new GLTexture();
            if (!depthTex->Create(width, height, depthFormat)) {
                USE_LOG_ERROR("GLFramebuffer: Failed to create depth/stencil texture");
                delete depthTex;
                glDeleteFramebuffersEXT(1, &m_fbo);
                m_fbo = 0;
                delete colorTex;
                m_colorAttachments[0] = nullptr;
                return false;
            }
            m_depthStencilAttachment = depthTex;
            // Attach depth and/or stencil based on format
            GLenum attachment = (depthFormat == TextureFormat::D24_UNORM_S8_UINT ||
                                 depthFormat == TextureFormat::D32_FLOAT_S8_UINT) ?
                                GL_DEPTH_STENCIL_ATTACHMENT_EXT : GL_DEPTH_ATTACHMENT_EXT;
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
                                      GL_TEXTURE_2D, depthTex->GetHandle(), 0);
        }

        if (!CheckStatus()) {
            Destroy();
            return false;
        }

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        USE_LOG_INFO("GLFramebuffer created: %dx%d", width, height);
        return true;
    }

    bool GLFramebuffer::AttachColorTexture(GLTexture* texture, int attachmentIndex)
    {
        if (!InitFBOPointers()) return false;
        if (!m_fbo) {
            USE_LOG_ERROR("GLFramebuffer::AttachColorTexture: framebuffer not created");
            return false;
        }
        if (attachmentIndex < 0 || attachmentIndex >= 8) {
            USE_LOG_ERROR("GLFramebuffer::AttachColorTexture: invalid attachment index");
            return false;
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + attachmentIndex,
                                  GL_TEXTURE_2D, texture->GetHandle(), 0);
        m_colorAttachments[attachmentIndex] = texture;
        if (attachmentIndex + 1 > m_numColorAttachments)
            m_numColorAttachments = attachmentIndex + 1;
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return true;
    }

    bool GLFramebuffer::AttachDepthStencilTexture(GLTexture* texture)
    {
        if (!InitFBOPointers()) return false;
        if (!m_fbo) {
            USE_LOG_ERROR("GLFramebuffer::AttachDepthStencilTexture: framebuffer not created");
            return false;
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        // Determine if texture has stencil
        TextureFormat fmt = texture->GetFormat();
        GLenum attachment = (fmt == TextureFormat::D24_UNORM_S8_UINT ||
                             fmt == TextureFormat::D32_FLOAT_S8_UINT) ?
                            GL_DEPTH_STENCIL_ATTACHMENT_EXT : GL_DEPTH_ATTACHMENT_EXT;
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
                                  GL_TEXTURE_2D, texture->GetHandle(), 0);
        m_depthStencilAttachment = texture;
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return true;
    }

    void GLFramebuffer::Bind()
    {
        if (m_fbo) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        }
    }

    void GLFramebuffer::Unbind()
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

    bool GLFramebuffer::IsComplete() const
    {
        if (!m_fbo) return false;
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return status == GL_FRAMEBUFFER_COMPLETE_EXT;
    }

    GLTexture* GLFramebuffer::GetColorTexture(int index) const
    {
        if (index >= 0 && index < m_numColorAttachments)
            return m_colorAttachments[index];
        return nullptr;
    }

    GLTexture* GLFramebuffer::GetDepthStencilTexture() const
    {
        return m_depthStencilAttachment;
    }

    bool GLFramebuffer::Resize(int width, int height)
    {
        if (!m_fbo) return false;
        if (m_width == width && m_height == height) return true;

        // Recreate all attached textures with new size.
        // This is a simple implementation; for better performance you might reuse textures.
        // We'll just create new textures and attach them.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        // Resize color attachments
        for (int i = 0; i < m_numColorAttachments; ++i) {
            if (m_colorAttachments[i]) {
                TextureFormat fmt = m_colorAttachments[i]->GetFormat();
                if (!m_colorAttachments[i]->Create(width, height, fmt)) {
                    USE_LOG_ERROR("GLFramebuffer::Resize: failed to resize color attachment");
                    return false;
                }
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i,
                                          GL_TEXTURE_2D, m_colorAttachments[i]->GetHandle(), 0);
            }
        }

        // Resize depth/stencil attachment
        if (m_depthStencilAttachment) {
            TextureFormat fmt = m_depthStencilAttachment->GetFormat();
            if (!m_depthStencilAttachment->Create(width, height, fmt)) {
                USE_LOG_ERROR("GLFramebuffer::Resize: failed to resize depth/stencil attachment");
                return false;
            }
            GLenum attachment = (fmt == TextureFormat::D24_UNORM_S8_UINT ||
                                 fmt == TextureFormat::D32_FLOAT_S8_UINT) ?
                                GL_DEPTH_STENCIL_ATTACHMENT_EXT : GL_DEPTH_ATTACHMENT_EXT;
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
                                      GL_TEXTURE_2D, m_depthStencilAttachment->GetHandle(), 0);
        }

        if (!CheckStatus()) {
            return false;
        }

        m_width = width;
        m_height = height;
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        USE_LOG_INFO("GLFramebuffer resized: %dx%d", width, height);
        return true;
    }

    void GLFramebuffer::Clear(uint32_t flags, const Color& color, float depth, uint32_t stencil)
    {
        Bind();
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
        Unbind();
    }

    void GLFramebuffer::Destroy()
    {
        if (m_fbo) {
            glDeleteFramebuffersEXT(1, &m_fbo);
            m_fbo = 0;
        }
        for (int i = 0; i < 8; ++i) {
            delete m_colorAttachments[i];
            m_colorAttachments[i] = nullptr;
        }
        delete m_depthStencilAttachment;
        m_depthStencilAttachment = nullptr;
        m_numColorAttachments = 0;
        m_width = m_height = 0;
    }

    bool GLFramebuffer::CheckStatus()
    {
        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
            const char* reason = "Unknown";
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                    reason = "Incomplete attachment";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                    reason = "Missing attachment";
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                    reason = "Dimensions mismatch";
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                    reason = "Unsupported format";
                    break;
            }
            USE_LOG_ERROR("GLFramebuffer incomplete: %s", reason);
            return false;
        }
        return true;
    }

} // namespace USE