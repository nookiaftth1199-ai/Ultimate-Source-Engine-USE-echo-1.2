// ============================================================
// Ultimate Source Engine - Console Renderer
// ============================================================
//
// Responsible for drawing the developer console on screen.
// Uses immediate mode OpenGL for simplicity (can be replaced with
// a more abstract 2D renderer later).
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "UI/Font.h"

namespace USE {

    // Forward declarations
    class ConsoleSystem;

    // Color scheme for console elements
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

    class ConsoleRender {
    public:
        ConsoleRender();
        ~ConsoleRender();

        // Initialize with a font
        bool Initialize(Font* font);

        // Render the console
        void Render(ConsoleSystem* console, int windowWidth, int windowHeight,
                    float heightPercentage, int scrollOffset,
                    const std::string& input, int cursorPos,
                    int selStart, int selEnd);

        // Set color scheme
        void SetColorScheme(const ConsoleColorScheme& scheme) { m_colorScheme = scheme; }

    private:
        Font* m_font;
        ConsoleColorScheme m_colorScheme;

        // Helper methods
        void RenderBackground(int width, int height, float consoleHeight);
        void RenderMessages(ConsoleSystem* console, int width, int consoleHeight,
                            int scrollOffset);
        void RenderInputLine(int width, int consoleHeight,
                             const std::string& input, int cursorPos,
                             int selStart, int selEnd);
        void RenderCursor(int cursorX, int cursorY, int lineHeight);
        void RenderSelection(int selStartX, int selEndX, int y, int height);
        void RenderScrollbar(int width, int consoleHeight, int messageCount,
                             int visibleLines, int scrollOffset);
        void RenderAutoComplete(int width, int consoleHeight,
                                const std::vector<std::string>& suggestions,
                                int selectedIndex);
    };

} // namespace USE