// ============================================================
// Ultimate Source Engine - Window Factory Implementation
// ============================================================

#include "stdafx.h"
#include "Window.h"

// Include the concrete SDL window implementation
#include "SDLWindow.h"

namespace USE {

    // -----------------------------------------------------------------
    // Factory method: creates a platform-specific window
    // -----------------------------------------------------------------
    Window* Window::Create(const WindowDesc& desc)
    {
        // Currently we only have an SDL implementation.
        // In the future, we could add Win32 or other backends.
        Window* window = new SDLWindow(desc);

        if (window && !window->Initialize()) {
            delete window;
            return nullptr;
        }

        return window;
    }

} // namespace USE