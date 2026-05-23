// ============================================================
// Ultimate Source Engine - Console User Interface
// ============================================================
//
// Handles the visual representation of the developer console,
// including background, text rendering, input line, cursor,
// scrollbar, and auto‑complete suggestions.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"

namespace USE {

    class ConsoleSystem;

    // Console color scheme (already defined in ConsoleSystem, but we can reuse)
    struct ConsoleColorScheme {
        Color background;
        Color border;
        Color inputBackground;
        Color inputText;
        Color cursor;
        Color selection;
        Color scrollbar;
        Color scrollbarThumb;
        Color messageColors[static_cast<int>(ConsoleMessageType::Count)];

        static ConsoleColorScheme DefaultSourceStyle();
        static ConsoleColorScheme DefaultDarkTheme();
        static ConsoleColorScheme DefaultLightTheme();
    };

    class ConsoleUI {
    public:
        ConsoleUI();
        ~ConsoleUI();

        // Initialize with render device and font
        bool Initialize(IRenderDevice* device, Font* font);
        void Shutdown();

        // Render the console (called when console is open)
        void Render(ConsoleSystem* console, float heightPercentage);

        // Set the color scheme
        void SetColorScheme(const ConsoleColorScheme& scheme);

        // Handle window resize (recompute layout)
        void OnResize(int width, int height);

    private:
        IRenderDevice* m_device;
        Font*          m_font;
        int            m_windowWidth;
        int            m_windowHeight;
        ConsoleColorScheme m_colorScheme;

        // Pre‑computed layout values (updated on resize)
        float m_fontSize;
        float m_lineHeight;
        int   m_inputHeight;
        int   m_visibleLines;
        int   m_autoCompleteMaxLines;

        // Helper rendering methods
        void RenderBackground(float height);
        void RenderMessages(ConsoleSystem* console, int scrollOffset, int consoleHeight);
        void RenderInputLine(const std::string& input, int cursorPos, int selStart, int selEnd, int consoleHeight);
        void RenderCursor(int cursorX, int cursorY);
        void RenderSelection(int selStartX, int selEndX, int y, int height);
        void RenderScrollbar(int consoleHeight, int messageCount, int visibleLines, int scrollOffset);
        void RenderAutoComplete(const std::vector<std::string>& suggestions, int selectedIndex, int consoleHeight);
    };

} // namespace USE