// ============================================================
// Ultimate Source Engine - SDL Window Implementation
// ============================================================
//
// This class implements the Window interface using SDL2.
// It handles window creation, OpenGL context management,
// event pumping, and basic window operations.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Window.h"
#include <SDL.h>

namespace USE {

    class SDLWindow : public Window {
    public:
        // Constructor takes window description
        explicit SDLWindow(const WindowDesc& desc);
        virtual ~SDLWindow();

        // Window interface implementation
        bool Initialize() override;
        void Shutdown() override;
        void PumpEvents() override;

        void Show() override;
        void Hide() override;

        int GetWidth() const override { return m_width; }
        int GetHeight() const override { return m_height; }
        void SetSize(int width, int height) override;

        int GetX() const override;
        int GetY() const override;
        void SetPosition(int x, int y) override;

        const std::string& GetTitle() const override { return m_title; }
        void SetTitle(const std::string& title) override;

        void SetFullscreen(bool fullscreen) override;
        bool IsFullscreen() const override;

        void SetVSync(bool enabled) override;
        bool IsVSyncEnabled() const override { return m_vsync; }

        bool IsVisible() const override { return m_visible; }
        bool IsFocused() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;

        void* GetNativeHandle() const override { return m_window; }
        void* GetSDLWindow() const override { return m_window; }

        void SetClipboardText(const std::string& text) override;
        std::string GetClipboardText() const override;

        void SetMouseCapture(bool capture) override;
        void SetRelativeMouseMode(bool enabled) override;
        void ShowCursor(bool show) override;
        void SetCursorPosition(int x, int y) override;

    private:
        SDL_Window*   m_window;
        SDL_GLContext m_glContext;
        std::string   m_title;
        int           m_width;
        int           m_height;
        bool          m_fullscreen;
        bool          m_resizable;
        bool          m_borderless;
        bool          m_vsync;
        bool          m_visible;
        bool          m_initialized;

        // OpenGL version requested
        int m_glMajor;
        int m_glMinor;

        // Internal helper
        void UpdateFullscreenFlag();
    };

} // namespace USE