// ======================================================
// Ultimate Source Engine - Win32 Window Header
// Platform-specific window implementation for Windows
// ======================================================

#ifndef ULTIMATE_SOURCE_WIN32_WINDOW_H
#define ULTIMATE_SOURCE_WIN32_WINDOW_H

#include "window.h"
#include <windows.h>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <vector>

namespace UltimateSource {

// ======================================================
// WINDOW STYLE CONSTANTS
// ======================================================

enum WindowStyle : DWORD
{
    WINDOW_STYLE_DEFAULT = WS_OVERLAPPEDWINDOW,
    WINDOW_STYLE_BORDERLESS = WS_POPUP,
    WINDOW_STYLE_FULLSCREEN = WS_POPUP | WS_VISIBLE,
    WINDOW_STYLE_CHILD = WS_CHILD | WS_VISIBLE,
    WINDOW_STYLE_TOOL = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
};

enum WindowExStyle : DWORD
{
    WINDOW_EX_STYLE_DEFAULT = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
    WINDOW_EX_STYLE_TOOL = WS_EX_TOOLWINDOW,
    WINDOW_EX_STYLE_TOPMOST = WS_EX_TOPMOST,
    WINDOW_EX_STYLE_TRANSPARENT = WS_EX_TRANSPARENT,
    WINDOW_EX_STYLE_LAYERED = WS_EX_LAYERED,
    WINDOW_EX_STYLE_ACCEPTFILES = WS_EX_ACCEPTFILES
};

// ======================================================
// WINDOW MESSAGE HANDLER
// ======================================================

using WindowMessageHandler = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

// ======================================================
// WIN32 WINDOW CLASS
// ======================================================

class ULTIMATE_SOURCE_API Win32Window : public Window
{
public:
    // ==================================================
    // CONSTRUCTOR & DESTRUCTOR
    // ==================================================
    
    Win32Window(const std::string& title, int width, int height);
    explicit Win32Window(const WindowSettings& settings);
    virtual ~Win32Window() override;

    // Disable copy
    Win32Window(const Win32Window&) = delete;
    Win32Window& operator=(const Win32Window&) = delete;

    // ==================================================
    // WINDOW LIFECYCLE
    // ==================================================
    
    virtual bool Create() override;
    virtual void Destroy() override;
    virtual void Close() override;
    virtual void Show(int nCmdShow = SW_SHOW) override;
    virtual void Hide() override;
    
    // ==================================================
    // WINDOW STATE
    // ==================================================
    
    virtual bool IsOpen() const override { return m_isOpen; }
    virtual bool IsVisible() const override { return m_isVisible; }
    virtual bool IsFocused() const override { return m_isFocused; }
    virtual bool ShouldClose() const override { return m_shouldClose; }
    
    // ==================================================
    // WINDOW PROPERTIES
    // ==================================================
    
    virtual void SetTitle(const std::string& title) override;
    virtual std::string GetTitle() const override { return m_title; }
    
    virtual void SetPosition(int x, int y) override;
    virtual void GetPosition(int& x, int& y) const override;
    
    virtual void SetSize(int width, int height) override;
    virtual void GetSize(int& width, int& height) const override;
    virtual int GetWidth() const override { return m_width; }
    virtual int GetHeight() const override { return m_height; }
    
    virtual void SetMinimumSize(int minWidth, int minHeight) override;
    virtual void GetMinimumSize(int& minWidth, int& minHeight) const override;
    
    virtual void SetMaximumSize(int maxWidth, int maxHeight) override;
    virtual void GetMaximumSize(int& maxWidth, int& maxHeight) const override;
    
    virtual void SetAspectRatio(int width, int height) override;
    
    // ==================================================
    // WINDOW MODE
    // ==================================================
    
    virtual void SetFullscreen(bool fullscreen) override;
    virtual bool IsFullscreen() const override { return m_isFullscreen; }
    virtual void ToggleFullscreen() override;
    
    virtual void SetBorderless(bool borderless) override;
    virtual bool IsBorderless() const override { return m_isBorderless; }
    
    virtual void SetResizable(bool resizable) override;
    virtual bool IsResizable() const override;
    
    virtual void SetAlwaysOnTop(bool alwaysOnTop) override;
    virtual bool IsAlwaysOnTop() const override;
    
    // ==================================================
    // WINDOW INPUT
    // ==================================================
    
    virtual void* GetHandle() const override { return m_hWnd; }
    virtual HWND GetHWND() const { return m_hWnd; }
    virtual HINSTANCE GetHInstance() const { return m_hInstance; }
    virtual HDC GetDC() const;
    
    virtual void ProcessEvents() override;
    virtual void SetMessageHandler(WindowMessageHandler handler);
    
    // ==================================================
    // WINDOW APPEARANCE
    // ==================================================
    
    virtual void SetIcon(const std::string& iconPath) override;
    virtual void SetCursor(StandardCursor cursor) override;
    virtual void ShowCursor(bool show) override;
    virtual void SetOpacity(float opacity) override;
    virtual float GetOpacity() const override { return m_opacity; }
    
    // ==================================================
    // WINDOW UTILITIES
    // ==================================================
    
    virtual void CenterOnScreen() override;
    virtual void FlashWindow() override;
    virtual void Restore() override;
    virtual void Minimize() override;
    virtual void Maximize() override;
    virtual bool IsMinimized() const;
    virtual bool IsMaximized() const;
    
    // ==================================================
    // CLIPBOARD
    // ==================================================
    
    virtual void SetClipboardText(const std::string& text) override;
    virtual std::string GetClipboardText() const override;
    virtual bool HasClipboardText() const override;
    
    // ==================================================
    // MONITOR INFORMATION
    // ==================================================
    
    virtual int GetMonitorCount() const override;
    virtual void GetMonitorSize(int& width, int& height) const override;
    virtual void GetWorkAreaSize(int& width, int& height) const override;
    virtual float GetDPIScale() const override;
    
    // ==================================================
    // STATIC UTILITIES
    // ==================================================
    
    static void InitializeApplication(HINSTANCE hInstance);
    static void ShutdownApplication();
    static HINSTANCE GetAppInstance() { return s_hInstance; }
    static std::string GetLastErrorMessage(DWORD errorCode = 0);
    
private:
    // ==================================================
    // PRIVATE MEMBERS
    // ==================================================
    
    // Window handles
    HWND m_hWnd;
    HINSTANCE m_hInstance;
    HDC m_hDC;
    
    // Window properties
    std::string m_title;
    int m_width, m_height;
    int m_minWidth, m_minHeight;
    int m_maxWidth, m_maxHeight;
    int m_aspectRatioWidth, m_aspectRatioHeight;
    float m_opacity;
    
    // Window state
    std::atomic<bool> m_isOpen;
    std::atomic<bool> m_isVisible;
    std::atomic<bool> m_isFocused;
    std::atomic<bool> m_shouldClose;
    std::atomic<bool> m_isFullscreen;
    std::atomic<bool> m_isBorderless;
    std::atomic<bool> m_isResizable;
    
    // Fullscreen state storage
    RECT m_windowedRect;
    DWORD m_windowedStyle;
    DWORD m_windowedExStyle;
    
    // Window class
    static const char* WINDOW_CLASS_NAME;
    static HINSTANCE s_hInstance;
    static bool s_classRegistered;
    
    // Message handler
    WindowMessageHandler m_customMessageHandler;
    std::mutex m_messageHandlerMutex;
    
    // Cursor
    HCURSOR m_currentCursor;
    
    // ==================================================
    // PRIVATE METHODS
    // ==================================================
    
    bool RegisterWindowClass();
    bool CreateWindowHandle();
    void InitializeMembers();
    void ApplyWindowStyles();
    void UpdateWindowRegion();
    
    // Message handling
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // Specific message handlers
    LRESULT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnEraseBackground(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnRButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnDisplayChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnDPIChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnEnterSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnExitSizeMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT OnGetMinMaxInfo(HWND hWnd, WPARAM wParam, LPARAM lParam);
    
    // Utility methods
    void UpdateAspectRatio(LPARAM lParam);
    void EnterFullscreen();
    void ExitFullscreen();
    void ApplyAspectRatioConstraint(int& width, int& height) const;
    bool IsAspectRatioConstrained() const;
    
    // DPI awareness
    static float GetDPIScaleForWindow(HWND hWnd);
    void OnDPIScaleChanged(float newScale);
};

// ======================================================
// INLINE IMPLEMENTATIONS
// ======================================================

inline bool Win32Window::IsResizable() const
{
    if (!m_hWnd) return m_isResizable;
    LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
    return (style & WS_THICKFRAME) != 0;
}

inline bool Win32Window::IsAlwaysOnTop() const
{
    if (!m_hWnd) return false;
    LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
    return (exStyle & WS_EX_TOPMOST) != 0;
}

inline bool Win32Window::IsMinimized() const
{
    return m_hWnd ? IsIconic(m_hWnd) != FALSE : false;
}

inline bool Win32Window::IsMaximized() const
{
    return m_hWnd ? IsZoomed(m_hWnd) != FALSE : false;
}

inline bool Win32Window::IsAspectRatioConstrained() const
{
    return m_aspectRatioWidth > 0 && m_aspectRatioHeight > 0;
}

} // namespace UltimateSource

#endif // ULTIMATE_SOURCE_WIN32_WINDOW_H#pragma