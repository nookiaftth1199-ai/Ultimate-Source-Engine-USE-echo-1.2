// ============================================================
// Ultimate Source Engine - Console Renderer Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleRender.h"
#include "ConsoleSystem.h"
#include "Math/MathUtils.h"
#include <GL/gl.h>
#include <GL/glu.h>

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
        // ... add others as needed
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
    // ConsoleRender implementation
    // -----------------------------------------------------------------
    ConsoleRender::ConsoleRender()
        : m_font(nullptr)
    {
        m_colorScheme = ConsoleColorScheme::DefaultSourceStyle();
    }

    ConsoleRender::~ConsoleRender()
    {
    }

    bool ConsoleRender::Initialize(Font* font)
    {
        m_font = font;
        return true;
    }

    void ConsoleRender::Render(ConsoleSystem* console, int windowWidth, int windowHeight,
                               float heightPercentage, int scrollOffset,
                               const std::string& input, int cursorPos,
                               int selStart, int selEnd)
    {
        if (!m_font) return;

        int consoleHeight = static_cast<int>(windowHeight * heightPercentage);
        int messageCount = static_cast<int>(console->GetFilteredMessages().size());

        // Save OpenGL state
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        RenderBackground(windowWidth, windowHeight, consoleHeight);
        RenderMessages(console, windowWidth, consoleHeight, scrollOffset);
        RenderInputLine(windowWidth, consoleHeight, input, cursorPos, selStart, selEnd);
        RenderScrollbar(windowWidth, consoleHeight, messageCount, 20, scrollOffset);

        auto suggestions = console->GetAutoCompleteSuggestions();
        if (!suggestions.empty()) {
            RenderAutoComplete(windowWidth, consoleHeight, suggestions,
                               console->GetAutoCompleteIndex());
        }

        // Restore OpenGL state
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
    }

    void ConsoleRender::RenderBackground(int width, int height, float consoleHeight)
    {
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.background.r, m_colorScheme.background.g,
                  m_colorScheme.background.b, m_colorScheme.background.a);
        glVertex2f(0, 0);
        glVertex2f(width, 0);
        glVertex2f(width, consoleHeight);
        glVertex2f(0, consoleHeight);
        glEnd();

        glBegin(GL_LINES);
        glColor4f(m_colorScheme.border.r, m_colorScheme.border.g,
                  m_colorScheme.border.b, m_colorScheme.border.a);
        glVertex2f(0, consoleHeight);
        glVertex2f(width, consoleHeight);
        glEnd();
    }

    void ConsoleRender::RenderMessages(ConsoleSystem* console, int width, int consoleHeight,
                                       int scrollOffset)
    {
        auto messages = console->GetFilteredMessages();
        int messageCount = static_cast<int>(messages.size());
        int visibleLines = 20; // could be computed from consoleHeight / lineHeight
        int lineHeight = static_cast<int>(m_font->GetLineHeight());

        int startIndex = MathUtils::Max(0, messageCount - visibleLines - scrollOffset);
        int endIndex = MathUtils::Min(messageCount, startIndex + visibleLines + scrollOffset);

        float y = 10.0f;

        for (int i = endIndex - 1; i >= startIndex; --i) {
            const auto& msg = messages[i];
            const Color& color = m_colorScheme.messageColors[static_cast<int>(msg.type)];

            std::string displayText = msg.text;
            if (msg.repeatCount > 1) {
                displayText += " (x" + std::to_string(msg.repeatCount) + ")";
            }

            m_font->RenderText(5, static_cast<int>(y), displayText, color);

            y += lineHeight;
            if (y > consoleHeight - 30) break; // leave space for input line
        }
    }

    void ConsoleRender::RenderInputLine(int width, int consoleHeight,
                                        const std::string& input, int cursorPos,
                                        int selStart, int selEnd)
    {
        int inputY = consoleHeight - 30;
        int lineHeight = static_cast<int>(m_font->GetLineHeight());

        // Input background
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.inputBackground.r, m_colorScheme.inputBackground.g,
                  m_colorScheme.inputBackground.b, m_colorScheme.inputBackground.a);
        glVertex2f(0, inputY);
        glVertex2f(width, inputY);
        glVertex2f(width, consoleHeight);
        glVertex2f(0, consoleHeight);
        glEnd();

        std::string prompt = "> ";
        std::string fullText = prompt + input;

        int promptWidth = m_font->GetTextWidth(prompt);
        int textBeforeCursor = m_font->GetTextWidth(input.substr(0, cursorPos));
        int cursorX = 5 + promptWidth + textBeforeCursor;
        int cursorY = inputY + (30 - lineHeight) / 2;

        // Selection
        if (selStart != selEnd) {
            int s1 = MathUtils::Min(selStart, selEnd);
            int s2 = MathUtils::Max(selStart, selEnd);
            int preSelWidth = m_font->GetTextWidth(input.substr(0, s1));
            int selWidth = m_font->GetTextWidth(input.substr(s1, s2 - s1));
            int selStartX = 5 + promptWidth + preSelWidth;
            int selEndX = selStartX + selWidth;
            RenderSelection(selStartX, selEndX, cursorY, lineHeight);
        }

        m_font->RenderText(5, cursorY, fullText, m_colorScheme.inputText);
        RenderCursor(cursorX, cursorY, lineHeight);
    }

    void ConsoleRender::RenderCursor(int cursorX, int cursorY, int lineHeight)
    {
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.cursor.r, m_colorScheme.cursor.g,
                  m_colorScheme.cursor.b, m_colorScheme.cursor.a);
        glVertex2f(cursorX, cursorY);
        glVertex2f(cursorX + 2, cursorY);
        glVertex2f(cursorX + 2, cursorY + lineHeight);
        glVertex2f(cursorX, cursorY + lineHeight);
        glEnd();
    }

    void ConsoleRender::RenderSelection(int selStartX, int selEndX, int y, int height)
    {
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.selection.r, m_colorScheme.selection.g,
                  m_colorScheme.selection.b, m_colorScheme.selection.a);
        glVertex2f(selStartX, y);
        glVertex2f(selEndX, y);
        glVertex2f(selEndX, y + height);
        glVertex2f(selStartX, y + height);
        glEnd();
    }

    void ConsoleRender::RenderScrollbar(int width, int consoleHeight, int messageCount,
                                        int visibleLines, int scrollOffset)
    {
        if (messageCount <= visibleLines) return;

        int scrollbarWidth = 8;
        float thumbHeight = static_cast<float>(consoleHeight) *
                            static_cast<float>(visibleLines) / static_cast<float>(messageCount);
        float thumbY = static_cast<float>(scrollOffset) /
                       static_cast<float>(messageCount - visibleLines) *
                       (consoleHeight - thumbHeight);

        // Track
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.scrollbar.r, m_colorScheme.scrollbar.g,
                  m_colorScheme.scrollbar.b, m_colorScheme.scrollbar.a);
        glVertex2f(width - scrollbarWidth, 0);
        glVertex2f(width, 0);
        glVertex2f(width, consoleHeight);
        glVertex2f(width - scrollbarWidth, consoleHeight);
        glEnd();

        // Thumb
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.scrollbarThumb.r, m_colorScheme.scrollbarThumb.g,
                  m_colorScheme.scrollbarThumb.b, m_colorScheme.scrollbarThumb.a);
        glVertex2f(width - scrollbarWidth, thumbY);
        glVertex2f(width, thumbY);
        glVertex2f(width, thumbY + thumbHeight);
        glVertex2f(width - scrollbarWidth, thumbY + thumbHeight);
        glEnd();
    }

    void ConsoleRender::RenderAutoComplete(int width, int consoleHeight,
                                           const std::vector<std::string>& suggestions,
                                           int selectedIndex)
    {
        if (suggestions.empty() || !m_font) return;

        int lineHeight = static_cast<int>(m_font->GetLineHeight());
        int boxX = 5;
        int boxY = consoleHeight - 30 - 10 - lineHeight * 10 - 6;
        int boxWidth = 300;
        int boxHeight = lineHeight * 10 + 6;

        // Background
        glBegin(GL_QUADS);
        glColor4f(0.1f, 0.1f, 0.15f, 0.95f);
        glVertex2f(boxX, boxY);
        glVertex2f(boxX + boxWidth, boxY);
        glVertex2f(boxX + boxWidth, boxY + boxHeight);
        glVertex2f(boxX, boxY + boxHeight);
        glEnd();

        int y = boxY + 3;
        for (size_t i = 0; i < suggestions.size() && i < 10; ++i) {
            Color color = (static_cast<int>(i) == selectedIndex)
                        ? Color(1.0f, 1.0f, 0.0f, 1.0f)
                        : Color(0.8f, 0.8f, 0.8f, 1.0f);
            m_font->RenderText(boxX + 5, y, suggestions[i], color);
            y += lineHeight;
        }
    }

} // namespace USE