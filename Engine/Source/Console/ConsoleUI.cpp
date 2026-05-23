// ============================================================
// Ultimate Source Engine - Console User Interface Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleUI.h"
#include "ConsoleSystem.h"
#include "Core/Logger.h"
#include "Math/MathUtils.h"
#include <GL/gl.h> // Temporary for immediate mode rendering – replace with proper render device calls later

namespace USE {

    // -----------------------------------------------------------------
    // Default color schemes
    // -----------------------------------------------------------------
    ConsoleColorScheme ConsoleColorScheme::DefaultSourceStyle()
    {
        ConsoleColorScheme scheme;
        scheme.background = Color(0.0f, 0.0f, 0.0f, 0.85f);
        scheme.border = Color(0.3f, 0.3f, 0.3f, 1.0f);
        scheme.inputBackground = Color(0.1f, 0.1f, 0.1f, 0.95f);
        scheme.inputText = Color(1.0f, 1.0f, 1.0f, 1.0f);
        scheme.cursor = Color(1.0f, 1.0f, 1.0f, 0.8f);
        scheme.selection = Color(0.2f, 0.4f, 0.8f, 0.5f);
        scheme.scrollbar = Color(0.2f, 0.2f, 0.2f, 0.8f);
        scheme.scrollbarThumb = Color(0.5f, 0.5f, 0.5f, 0.9f);

        scheme.messageColors[static_cast<int>(ConsoleMessageType::Info)]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Warning)] = Color(1.0f, 1.0f, 0.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Error)]   = Color(1.0f, 0.3f, 0.3f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Command)] = Color(0.4f, 1.0f, 0.4f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Output)]  = Color(0.7f, 0.7f, 1.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Debug)]   = Color(0.5f, 0.5f, 0.5f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Success)] = Color(0.0f, 1.0f, 0.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::System)]  = Color(1.0f, 1.0f, 1.0f, 1.0f);
        // ... etc.
        return scheme;
    }

    ConsoleColorScheme ConsoleColorScheme::DefaultDarkTheme()
    {
        auto scheme = DefaultSourceStyle();
        scheme.background = Color(0.05f, 0.05f, 0.1f, 0.95f);
        scheme.inputBackground = Color(0.1f, 0.1f, 0.15f, 0.98f);
        scheme.border = Color(0.2f, 0.2f, 0.3f, 1.0f);
        return scheme;
    }

    ConsoleColorScheme ConsoleColorScheme::DefaultLightTheme()
    {
        auto scheme = DefaultSourceStyle();
        scheme.background = Color(0.95f, 0.95f, 1.0f, 0.95f);
        scheme.inputBackground = Color(0.9f, 0.9f, 0.95f, 0.98f);
        scheme.inputText = Color(0.0f, 0.0f, 0.0f, 1.0f);
        scheme.border = Color(0.3f, 0.3f, 0.4f, 1.0f);
        scheme.cursor = Color(0.0f, 0.0f, 0.0f, 0.8f);
        scheme.selection = Color(0.6f, 0.8f, 1.0f, 0.5f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Info)] = Color(0.0f, 0.0f, 0.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Warning)] = Color(0.8f, 0.6f, 0.0f, 1.0f);
        scheme.messageColors[static_cast<int>(ConsoleMessageType::Error)] = Color(0.8f, 0.0f, 0.0f, 1.0f);
        return scheme;
    }

    // -----------------------------------------------------------------
    // ConsoleUI implementation
    // -----------------------------------------------------------------
    ConsoleUI::ConsoleUI()
        : m_device(nullptr)
        , m_font(nullptr)
        , m_windowWidth(0)
        , m_windowHeight(0)
        , m_fontSize(16.0f)
        , m_lineHeight(20.0f)
        , m_inputHeight(30)
        , m_visibleLines(20)
        , m_autoCompleteMaxLines(10)
    {
        m_colorScheme = ConsoleColorScheme::DefaultSourceStyle();
    }

    ConsoleUI::~ConsoleUI()
    {
        Shutdown();
    }

    bool ConsoleUI::Initialize(IRenderDevice* device, Font* font)
    {
        m_device = device;
        m_font = font;
        if (!m_font) {
            USE_LOG_WARN("ConsoleUI: No font provided, text will not be rendered.");
        }
        return true;
    }

    void ConsoleUI::Shutdown()
    {
        // Nothing to release yet
    }

    void ConsoleUI::SetColorScheme(const ConsoleColorScheme& scheme)
    {
        m_colorScheme = scheme;
    }

    void ConsoleUI::OnResize(int width, int height)
    {
        m_windowWidth = width;
        m_windowHeight = height;
        // Recompute layout if needed
        if (m_font) {
            m_lineHeight = m_font->GetLineHeight();
            m_inputHeight = static_cast<int>(m_lineHeight + 8);
        }
    }

    void ConsoleUI::Render(ConsoleSystem* console, float heightPercentage)
    {
        if (!m_device || !console || heightPercentage <= 0.0f) return;

        int consoleHeight = static_cast<int>(m_windowHeight * heightPercentage);
        int scrollOffset = console->GetScrollOffset();
        int messageCount = static_cast<int>(console->GetFilteredMessages().size());

        // Draw background
        RenderBackground(consoleHeight);

        // Draw messages
        RenderMessages(console, scrollOffset, consoleHeight);

        // Draw input line
        RenderInputLine(console->GetInputBuffer(), console->GetCursorPos(),
                        console->GetSelectionStart(), console->GetSelectionEnd(),
                        consoleHeight);

        // Draw scrollbar
        RenderScrollbar(consoleHeight, messageCount, m_visibleLines, scrollOffset);

        // Draw auto-complete suggestions if any
        if (console->HasAutoComplete()) {
            RenderAutoComplete(console->GetAutoCompleteSuggestions(),
                               console->GetAutoCompleteIndex(),
                               consoleHeight);
        }
    }

    void ConsoleUI::RenderBackground(float height)
    {
        // Use immediate mode for now – later replace with proper quad rendering via device
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, m_windowWidth, m_windowHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Main background
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.background.r, m_colorScheme.background.g,
                  m_colorScheme.background.b, m_colorScheme.background.a);
        glVertex2f(0, 0);
        glVertex2f(m_windowWidth, 0);
        glVertex2f(m_windowWidth, height);
        glVertex2f(0, height);
        glEnd();

        // Bottom border line
        glBegin(GL_LINES);
        glColor4f(m_colorScheme.border.r, m_colorScheme.border.g,
                  m_colorScheme.border.b, m_colorScheme.border.a);
        glVertex2f(0, height);
        glVertex2f(m_windowWidth, height);
        glEnd();

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    void ConsoleUI::RenderMessages(ConsoleSystem* console, int scrollOffset, int consoleHeight)
    {
        if (!m_font) return;

        auto messages = console->GetFilteredMessages();
        int messageCount = static_cast<int>(messages.size());
        int startIndex = MathUtils::Max(0, messageCount - m_visibleLines - scrollOffset);
        int endIndex = MathUtils::Min(messageCount, startIndex + m_visibleLines + scrollOffset);

        float y = 10.0f;
        float lineHeight = m_lineHeight;

        // Setup ortho for 2D text rendering
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, m_windowWidth, m_windowHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = endIndex - 1; i >= startIndex; --i) {
            const auto& msg = messages[i];
            const Color& color = m_colorScheme.messageColors[static_cast<int>(msg.type)];

            std::string displayText;
            if (msg.repeatCount > 1) {
                displayText = msg.text + " (x" + std::to_string(msg.repeatCount) + ")";
            } else {
                displayText = msg.text;
            }

            // Render text using font
            m_font->RenderText(5, static_cast<int>(y), displayText, color);

            y += lineHeight;
            if (y > consoleHeight - m_inputHeight) break;
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    void ConsoleUI::RenderInputLine(const std::string& input, int cursorPos, int selStart, int selEnd, int consoleHeight)
    {
        int inputY = consoleHeight - m_inputHeight;

        // Input background
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, m_windowWidth, m_windowHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.inputBackground.r, m_colorScheme.inputBackground.g,
                  m_colorScheme.inputBackground.b, m_colorScheme.inputBackground.a);
        glVertex2f(0, inputY);
        glVertex2f(m_windowWidth, inputY);
        glVertex2f(m_windowWidth, consoleHeight);
        glVertex2f(0, consoleHeight);
        glEnd();

        glDisable(GL_BLEND);

        // Render input text
        if (m_font) {
            std::string prompt = "> ";
            std::string fullText = prompt + input;

            // Compute cursor position in pixels (rough approximation)
            int promptWidth = m_font->GetTextWidth(prompt);
            int textBeforeCursor = m_font->GetTextWidth(input.substr(0, cursorPos));
            int cursorX = 5 + promptWidth + textBeforeCursor;
            int cursorY = inputY + (m_inputHeight - static_cast<int>(m_lineHeight)) / 2;

            // Render selection if any
            if (selStart != selEnd) {
                int selStartX, selEndX;
                int s1 = MathUtils::Min(selStart, selEnd);
                int s2 = MathUtils::Max(selStart, selEnd);
                int preSelWidth = m_font->GetTextWidth(input.substr(0, s1));
                int selWidth = m_font->GetTextWidth(input.substr(s1, s2 - s1));
                selStartX = 5 + promptWidth + preSelWidth;
                selEndX = selStartX + selWidth;
                RenderSelection(selStartX, selEndX, cursorY, static_cast<int>(m_lineHeight));
            }

            // Draw text
            m_font->RenderText(5, cursorY, fullText, m_colorScheme.inputText);

            // Draw cursor
            RenderCursor(cursorX, cursorY);
        }

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    void ConsoleUI::RenderCursor(int cursorX, int cursorY)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.cursor.r, m_colorScheme.cursor.g,
                  m_colorScheme.cursor.b, m_colorScheme.cursor.a);
        glVertex2f(cursorX, cursorY);
        glVertex2f(cursorX + 2, cursorY);
        glVertex2f(cursorX + 2, cursorY + m_lineHeight);
        glVertex2f(cursorX, cursorY + m_lineHeight);
        glEnd();
        glDisable(GL_BLEND);
    }

    void ConsoleUI::RenderSelection(int selStartX, int selEndX, int y, int height)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.selection.r, m_colorScheme.selection.g,
                  m_colorScheme.selection.b, m_colorScheme.selection.a);
        glVertex2f(selStartX, y);
        glVertex2f(selEndX, y);
        glVertex2f(selEndX, y + height);
        glVertex2f(selStartX, y + height);
        glEnd();
        glDisable(GL_BLEND);
    }

    void ConsoleUI::RenderScrollbar(int consoleHeight, int messageCount, int visibleLines, int scrollOffset)
    {
        if (messageCount <= visibleLines) return;

        int scrollbarWidth = 8;
        float thumbHeight = static_cast<float>(consoleHeight) * static_cast<float>(visibleLines) / static_cast<float>(messageCount);
        float thumbY = static_cast<float>(scrollOffset) / static_cast<float>(messageCount - visibleLines) *
                       (consoleHeight - thumbHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Track
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.scrollbar.r, m_colorScheme.scrollbar.g,
                  m_colorScheme.scrollbar.b, m_colorScheme.scrollbar.a);
        glVertex2f(m_windowWidth - scrollbarWidth, 0);
        glVertex2f(m_windowWidth, 0);
        glVertex2f(m_windowWidth, consoleHeight);
        glVertex2f(m_windowWidth - scrollbarWidth, consoleHeight);
        glEnd();

        // Thumb
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.scrollbarThumb.r, m_colorScheme.scrollbarThumb.g,
                  m_colorScheme.scrollbarThumb.b, m_colorScheme.scrollbarThumb.a);
        glVertex2f(m_windowWidth - scrollbarWidth, thumbY);
        glVertex2f(m_windowWidth, thumbY);
        glVertex2f(m_windowWidth, thumbY + thumbHeight);
        glVertex2f(m_windowWidth - scrollbarWidth, thumbY + thumbHeight);
        glEnd();

        glDisable(GL_BLEND);
    }

    void ConsoleUI::RenderAutoComplete(const std::vector<std::string>& suggestions, int selectedIndex, int consoleHeight)
    {
        if (suggestions.empty() || !m_font) return;

        int boxX = 5;
        int boxY = consoleHeight - m_inputHeight - 10 - static_cast<int>(m_lineHeight * m_autoCompleteMaxLines) - 6;
        int boxWidth = 300; // could compute based on longest suggestion
        int boxHeight = static_cast<int>(m_lineHeight * m_autoCompleteMaxLines) + 6;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Background
        glBegin(GL_QUADS);
        glColor4f(0.1f, 0.1f, 0.15f, 0.95f);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxWidth, boxY);
        glVertex2f(boxX + boxWidth, boxY + boxHeight);
        glVertex2f(boxX, boxY + boxHeight);
        glEnd();

        // Suggestions text
        int y = boxY + 3;
        for (size_t i = 0; i < suggestions.size() && i < static_cast<size_t>(m_autoCompleteMaxLines); ++i) {
            Color textColor = (i == static_cast<size_t>(selectedIndex))
                                ? Color(1.0f, 1.0f, 0.0f, 1.0f)
                                : Color(0.8f, 0.8f, 0.8f, 1.0f);
            m_font->RenderText(boxX + 5, y, suggestions[i], textColor);
            y += static_cast<int>(m_lineHeight);
        }

        glDisable(GL_BLEND);
    }

} // namespace USE
