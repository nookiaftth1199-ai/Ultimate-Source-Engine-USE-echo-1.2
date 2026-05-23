// ============================================================
// Ultimate Source Engine - Window Abstraction
// ============================================================
//
// This header defines the abstract Window interface, which
// provides a platform-independent way to create and manage
// application windows.
//
// The actual implementation is platform-specific (SDL, Win32, etc.)
// and is created via the static Create() factory method.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector2.h"

namespace USE {

    // Window creation parameters
    struct WindowDesc {
        std::string title;
        int width;
        int height;
        bool fullscreen;
        bool resizable;
        bool borderless;
        bool vsync;
        int glMajorVersion;      // For OpenGL
        int glMinorVersion;
        
        WindowDesc()
            : title("Ultimate Source Engine")
            , width(1280)
            , height(720)
            , fullscreen(false)
            , resizable(true)
            , borderless(false)
            , vsync(true)
            , glMajorVersion(2)
            , glMinorVersion(0)
        {}
    };

    // Abstract window interface
    class Window {
    public:
        virtual ~Window() {}

        // Factory method: creates a platform-specific window
        static Window* Create(const WindowDesc& desc = WindowDesc());

        // Window control
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        // Main loop event pumping
        virtual void PumpEvents() = 0;

        // Show/hide
        virtual void Show() = 0;
        virtual void Hide() = 0;

        // Window properties
        virtual int  GetWidth() const = 0;
        virtual int  GetHeight() const = 0;
        virtual Vector2 GetSize() const { return Vector2((float)GetWidth(), (float)GetHeight()); }
        virtual void SetSize(int width, int height) = 0;

        virtual int  GetX() const = 0;
        virtual int  GetY() const = 0;
        virtual Vector2 GetPosition() const { return Vector2((float)GetX(), (float)GetY()); }
        virtual void SetPosition(int x, int y) = 0;

        virtual const std::string& GetTitle() const = 0;
        virtual void SetTitle(const std::string& title) = 0;

        // Fullscreen
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual bool IsFullscreen() const = 0;

        // VSync
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        // Window state
        virtual bool IsVisible() const = 0;
        virtual bool IsFocused() const = 0;
        virtual bool IsMinimized() const = 0;
        virtual bool IsMaximized() const = 0;

        // Platform-specific handle (HWND, NSWindow*, etc.)
        virtual void* GetNativeHandle() const = 0;

        // SDL-specific (if using SDL) – could be null for non-SDL implementations
        virtual void* GetSDLWindow() const { return nullptr; }

        // Clipboard
        virtual void SetClipboardText(const std::string& text) = 0;
        virtual std::string GetClipboardText() const = 0;

        // Input modes (mouse capture, relative mode, etc.)
        virtual void SetMouseCapture(bool capture) = 0;
        virtual void SetRelativeMouseMode(bool enabled) = 0;
        virtual void ShowCursor(bool show) = 0;
        virtual void SetCursorPosition(int x, int y) = 0;
    };

} // namespace USE