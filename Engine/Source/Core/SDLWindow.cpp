// ============================================================
// Ultimate Source Engine - SDL Window Implementation
// ============================================================

#include "stdafx.h"
#include "SDLWindow.h"
#include "Platform.h"
#include "Core/Logger.h"

#include <SDL.h>

namespace USE {

    // -----------------------------------------------------------------
    // Constructor
    // -----------------------------------------------------------------
    SDLWindow::SDLWindow(const WindowDesc& desc)
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_title(desc.title)
        , m_width(desc.width)
        , m_height(desc.height)
        , m_fullscreen(desc.fullscreen)
        , m_resizable(desc.resizable)
        , m_borderless(desc.borderless)
        , m_vsync(desc.vsync)
        , m_visible(false)
        , m_initialized(false)
        , m_glMajor(desc.glMajorVersion)
        , m_glMinor(desc.glMinorVersion)
    {
        USE_LOG_INFO("SDLWindow created with title: %s, size: %dx%d", 
                     m_title.c_str(), m_width, m_height);
    }

    // -----------------------------------------------------------------
    // Destructor
    // -----------------------------------------------------------------
    SDLWindow::~SDLWindow()
    {
        Shutdown();
    }

    // -----------------------------------------------------------------
    // Initialize the SDL window and OpenGL context
    // -----------------------------------------------------------------
    bool SDLWindow::Initialize()
    {
        if (m_initialized) {
            USE_LOG_WARN("SDLWindow already initialized");
            return true;
        }

        USE_LOG_INFO("Initializing SDLWindow...");

        // Initialize SDL video subsystem
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            USE_LOG_ERROR("SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_glMajor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_glMinor);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // Optional: request a specific profile (core/compatibility)
        // For OpenGL 2.0, we don't need to set a profile.

        // Window creation flags
        Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        if (m_resizable)   flags |= SDL_WINDOW_RESIZABLE;
        if (m_borderless)  flags |= SDL_WINDOW_BORDERLESS;
        if (m_fullscreen)  flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

        // Create window
        m_window = SDL_CreateWindow(
            m_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_width,
            m_height,
            flags
        );

        if (!m_window) {
            USE_LOG_ERROR("SDL_CreateWindow failed: %s", SDL_GetError());
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            return false;
        }

        // Create OpenGL context
        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext) {
            USE_LOG_ERROR("SDL_GL_CreateContext failed: %s", SDL_GetError());
            SDL_DestroyWindow(m_window);
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            return false;
        }

        // Set vsync
        SetVSync(m_vsync);

        // Get actual window size (might be different after creation)
        SDL_GetWindowSize(m_window, &m_width, &m_height);

        m_visible = true;
        m_initialized = true;

        USE_LOG_INFO("SDLWindow initialized successfully. OpenGL context created.");
        return true;
    }

    // -----------------------------------------------------------------
    // Shutdown: destroy window and quit SDL
    // -----------------------------------------------------------------
    void SDLWindow::Shutdown()
    {
        if (!m_initialized) return;

        USE_LOG_INFO("Shutting down SDLWindow...");

        if (m_glContext) {
            SDL_GL_DeleteContext(m_glContext);
            m_glContext = nullptr;
        }

        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        m_initialized = false;
        m_visible = false;

        USE_LOG_INFO("SDLWindow shutdown complete.");
    }

    // -----------------------------------------------------------------
    // Pump events – should be called once per frame
    // -----------------------------------------------------------------
    void SDLWindow::PumpEvents()
    {
        // This is handled by the application's event loop.
        // Here we just ensure SDL's internal event processing.
        SDL_PumpEvents();
    }

    // -----------------------------------------------------------------
    // Show/Hide window
    // -----------------------------------------------------------------
    void SDLWindow::Show()
    {
        if (!m_initialized) return;
        SDL_ShowWindow(m_window);
        m_visible = true;
    }

    void SDLWindow::Hide()
    {
        if (!m_initialized) return;
        SDL_HideWindow(m_window);
        m_visible = false;
    }

    // -----------------------------------------------------------------
    // Set window size
    // -----------------------------------------------------------------
    void SDLWindow::SetSize(int width, int height)
    {
        if (!m_initialized) return;
        SDL_SetWindowSize(m_window, width, height);
        m_width = width;
        m_height = height;
    }

    // -----------------------------------------------------------------
    // Get window position
    // -----------------------------------------------------------------
    int SDLWindow::GetX() const
    {
        int x, y;
        SDL_GetWindowPosition(m_window, &x, &y);
        return x;
    }

    int SDLWindow::GetY() const
    {
        int x, y;
        SDL_GetWindowPosition(m_window, &x, &y);
        return y;
    }

    // -----------------------------------------------------------------
    // Set window position
    // -----------------------------------------------------------------
    void SDLWindow::SetPosition(int x, int y)
    {
        if (!m_initialized) return;
        SDL_SetWindowPosition(m_window, x, y);
    }

    // -----------------------------------------------------------------
    // Set window title
    // -----------------------------------------------------------------
    void SDLWindow::SetTitle(const std::string& title)
    {
        if (!m_initialized) return;
        m_title = title;
        SDL_SetWindowTitle(m_window, title.c_str());
    }

    // -----------------------------------------------------------------
    // Fullscreen mode
    // -----------------------------------------------------------------
    void SDLWindow::SetFullscreen(bool fullscreen)
    {
        if (!m_initialized) return;
        if (fullscreen == m_fullscreen) return;

        Uint32 flag = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        if (SDL_SetWindowFullscreen(m_window, flag) == 0) {
            m_fullscreen = fullscreen;
        }
    }

    bool SDLWindow::IsFullscreen() const
    {
        if (!m_initialized) return false;
        Uint32 flags = SDL_GetWindowFlags(m_window);
        return (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
    }

    // -----------------------------------------------------------------
    // VSync
    // -----------------------------------------------------------------
    void SDLWindow::SetVSync(bool enabled)
    {
        if (!m_initialized) return;
        int result = SDL_GL_SetSwapInterval(enabled ? 1 : 0);
        if (result == 0) {
            m_vsync = enabled;
        } else {
            USE_LOG_WARN("SDL_GL_SetSwapInterval failed: %s", SDL_GetError());
        }
    }

    // -----------------------------------------------------------------
    // Window state
    // -----------------------------------------------------------------
    bool SDLWindow::IsFocused() const
    {
        if (!m_initialized) return false;
        Uint32 flags = SDL_GetWindowFlags(m_window);
        return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    bool SDLWindow::IsMinimized() const
    {
        if (!m_initialized) return false;
        Uint32 flags = SDL_GetWindowFlags(m_window);
        return (flags & SDL_WINDOW_MINIMIZED) != 0;
    }

    bool SDLWindow::IsMaximized() const
    {
        if (!m_initialized) return false;
        Uint32 flags = SDL_GetWindowFlags(m_window);
        return (flags & SDL_WINDOW_MAXIMIZED) != 0;
    }

    // -----------------------------------------------------------------
    // Clipboard
    // -----------------------------------------------------------------
    void SDLWindow::SetClipboardText(const std::string& text)
    {
        SDL_SetClipboardText(text.c_str());
    }

    std::string SDLWindow::GetClipboardText() const
    {
        char* text = SDL_GetClipboardText();
        if (text) {
            std::string result(text);
            SDL_free(text);
            return result;
        }
        return "";
    }

    // -----------------------------------------------------------------
    // Mouse capture
    // -----------------------------------------------------------------
    void SDLWindow::SetMouseCapture(bool capture)
    {
        SDL_CaptureMouse(capture ? SDL_TRUE : SDL_FALSE);
    }

    void SDLWindow::SetRelativeMouseMode(bool enabled)
    {
        SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    }

    void SDLWindow::ShowCursor(bool show)
    {
        SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
    }

    void SDLWindow::SetCursorPosition(int x, int y)
    {
        SDL_WarpMouseInWindow(m_window, x, y);
    }

    // -----------------------------------------------------------------
    // Internal: update fullscreen flag based on actual window flags
    // (used after window events)
    // -----------------------------------------------------------------
    void SDLWindow::UpdateFullscreenFlag()
    {
        if (m_initialized) {
            m_fullscreen = IsFullscreen();
        }
    }

} // namespace USE