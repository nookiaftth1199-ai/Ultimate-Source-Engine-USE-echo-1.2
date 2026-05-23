// ============================================================
// Ultimate Source Engine - UI Text Box Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UITextBox.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"
#include <algorithm>
#include <cctype>

namespace USE {

    UITextBox::UITextBox()
        : m_font(nullptr)
        , m_textColor(0,0,0,1)
        , m_placeholderColor(0.5f,0.5f,0.5f,1)
        , m_bgColor(1,1,1,1)
        , m_cursorColor(0,0,0,1)
        , m_selectionColor(0.5f,0.7f,1.0f,0.5f)
        , m_maxLength(0)
        , m_passwordMode(false)
        , m_readOnly(false)
        , m_cursorPos(0)
        , m_selectionStart(0)
        , m_selectionEnd(0)
        , m_cursorBlinkTimer(0.0f)
        , m_cursorVisible(true)
    {
    }

    void UITextBox::SetText(const std::string& text)
    {
        m_text = text;
        if (m_maxLength > 0 && m_text.length() > m_maxLength) {
            m_text.resize(m_maxLength);
        }
        m_cursorPos = m_text.length();
        m_selectionStart = m_selectionEnd = m_cursorPos;
    }

    void UITextBox::InsertCharacter(char c)
    {
        if (m_readOnly) return;
        if (m_maxLength > 0 && m_text.length() >= m_maxLength) return;

        // Handle printable characters only
        if (isprint(c)) {
            if (m_selectionStart != m_selectionEnd) {
                DeleteSelection();
            }
            m_text.insert(m_text.begin() + m_cursorPos, c);
            m_cursorPos++;
            m_selectionStart = m_selectionEnd = m_cursorPos;
        }
    }

    void UITextBox::DeleteCharacter(bool forward)
    {
        if (m_readOnly) return;
        if (m_selectionStart != m_selectionEnd) {
            DeleteSelection();
            return;
        }
        if (forward) {
            // Delete key
            if (m_cursorPos < m_text.length()) {
                m_text.erase(m_cursorPos, 1);
            }
        } else {
            // Backspace
            if (m_cursorPos > 0) {
                m_text.erase(m_cursorPos - 1, 1);
                m_cursorPos--;
            }
        }
        m_selectionStart = m_selectionEnd = m_cursorPos;
    }

    void UITextBox::DeleteSelection()
    {
        if (m_selectionStart == m_selectionEnd) return;
        size_t start = std::min(m_selectionStart, m_selectionEnd);
        size_t end = std::max(m_selectionStart, m_selectionEnd);
        m_text.erase(start, end - start);
        m_cursorPos = start;
        m_selectionStart = m_selectionEnd = m_cursorPos;
    }

    void UITextBox::MoveCursor(int direction, bool shiftPressed)
    {
        int newPos = static_cast<int>(m_cursorPos) + direction;
        newPos = std::max(0, std::min(static_cast<int>(m_text.length()), newPos));

        if (!shiftPressed) {
            m_cursorPos = newPos;
            m_selectionStart = m_selectionEnd = m_cursorPos;
        } else {
            // Extend selection
            m_cursorPos = newPos;
            m_selectionEnd = m_cursorPos;
        }
    }

    void UITextBox::MoveCursorWord(int direction, bool shiftPressed)
    {
        // Simple word boundary detection
        size_t pos = m_cursorPos;
        if (direction < 0) {
            // move left to previous word start
            while (pos > 0 && isspace(m_text[pos-1])) pos--;
            while (pos > 0 && !isspace(m_text[pos-1])) pos--;
        } else {
            // move right to next word start
            size_t len = m_text.length();
            while (pos < len && !isspace(m_text[pos])) pos++;
            while (pos < len && isspace(m_text[pos])) pos++;
        }
        if (!shiftPressed) {
            m_cursorPos = pos;
            m_selectionStart = m_selectionEnd = m_cursorPos;
        } else {
            m_cursorPos = pos;
            m_selectionEnd = m_cursorPos;
        }
    }

    void UITextBox::SelectAll()
    {
        m_selectionStart = 0;
        m_selectionEnd = m_text.length();
        m_cursorPos = m_selectionEnd;
    }

    bool UITextBox::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        if (button != 0) return false; // left button only

        if (down && ContainsPoint(x, y)) {
            // Request focus (handled by UISystem)
            // For now, we just set cursor position based on click.
            float localX = x - m_x;
            size_t charIndex = GetCharIndexAtPosition(localX);
            m_cursorPos = charIndex;
            m_selectionStart = m_selectionEnd = m_cursorPos;
            return true;
        }
        return false;
    }

    bool UITextBox::OnKeyPress(int key, bool down)
    {
        if (!m_visible || !m_enabled || !down) return false;

        bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
        bool ctrl = (SDL_GetModState() & KMOD_CTRL) != 0;

        switch (key) {
            case SDLK_BACKSPACE:
                DeleteCharacter(false);
                break;
            case SDLK_DELETE:
                DeleteCharacter(true);
                break;
            case SDLK_LEFT:
                if (ctrl)
                    MoveCursorWord(-1, shift);
                else
                    MoveCursor(-1, shift);
                break;
            case SDLK_RIGHT:
                if (ctrl)
                    MoveCursorWord(1, shift);
                else
                    MoveCursor(1, shift);
                break;
            case SDLK_HOME:
                MoveCursor(-static_cast<int>(m_cursorPos), shift);
                break;
            case SDLK_END:
                MoveCursor(static_cast<int>(m_text.length()) - static_cast<int>(m_cursorPos), shift);
                break;
            case SDLK_a:
                if (ctrl) {
                    SelectAll();
                }
                break;
            case SDLK_c:
                if (ctrl) {
                    // Copy selected text to clipboard (not implemented)
                }
                break;
            case SDLK_v:
                if (ctrl) {
                    // Paste from clipboard (not implemented)
                }
                break;
            case SDLK_x:
                if (ctrl) {
                    // Cut (copy + delete) (not implemented)
                }
                break;
            default:
                break;
        }
        return true;
    }

    bool UITextBox::OnCharInput(unsigned int codepoint)
    {
        if (!m_visible || !m_enabled || m_readOnly) return false;
        // Only accept printable characters
        if (codepoint >= 32 && codepoint < 128) {
            InsertCharacter(static_cast<char>(codepoint));
            return true;
        }
        return false;
    }

    void UITextBox::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        Font* fontToUse = m_font ? m_font : defaultFont;
        if (!fontToUse) return;

        // Draw background
        glBegin(GL_QUADS);
        glColor4f(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Determine what text to display
        std::string displayText;
        if (m_text.empty() && !m_placeholder.empty() && !m_readOnly) {
            displayText = m_placeholder;
        } else {
            displayText = GetDisplayText();
        }

        // Text position (left aligned, vertically centered)
        float textX = m_x + 5; // small padding
        float textY = m_y + (m_height - fontToUse->GetLineHeight()) * 0.5f;

        // Draw selection background if any
        if (m_selectionStart != m_selectionEnd && !m_text.empty() && displayText == GetDisplayText()) {
            size_t start = std::min(m_selectionStart, m_selectionEnd);
            size_t end = std::max(m_selectionStart, m_selectionEnd);
            std::string preSel = displayText.substr(0, start);
            std::string sel = displayText.substr(start, end - start);
            float preWidth = fontToUse->GetTextWidth(preSel);
            float selWidth = fontToUse->GetTextWidth(sel);
            glBegin(GL_QUADS);
            glColor4f(m_selectionColor.r, m_selectionColor.g, m_selectionColor.b, m_selectionColor.a);
            glVertex2f(textX + preWidth, textY);
            glVertex2f(textX + preWidth + selWidth, textY);
            glVertex2f(textX + preWidth + selWidth, textY + fontToUse->GetLineHeight());
            glVertex2f(textX + preWidth, textY + fontToUse->GetLineHeight());
            glEnd();
        }

        // Draw text
        Color textColor = (m_text.empty() && !m_placeholder.empty()) ? m_placeholderColor : m_textColor;
        fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY), displayText, textColor);

        // Draw cursor if focused (blinking) and not read-only
        if (m_focused && !m_readOnly && m_text.empty() == displayText.empty()) {
            // Only show cursor if the displayed text is the actual text (not placeholder)
            m_cursorBlinkTimer += 0.016f; // assume 60 FPS; better to pass deltaTime
            if (m_cursorBlinkTimer >= 0.5f) {
                m_cursorVisible = !m_cursorVisible;
                m_cursorBlinkTimer = 0.0f;
            }
            if (m_cursorVisible) {
                std::string preCursor = GetDisplayText().substr(0, m_cursorPos);
                float cursorX = textX + fontToUse->GetTextWidth(preCursor);
                glBegin(GL_QUADS);
                glColor4f(m_cursorColor.r, m_cursorColor.g, m_cursorColor.b, m_cursorColor.a);
                glVertex2f(cursorX, textY);
                glVertex2f(cursorX + 2, textY);
                glVertex2f(cursorX + 2, textY + fontToUse->GetLineHeight());
                glVertex2f(cursorX, textY + fontToUse->GetLineHeight());
                glEnd();
            }
        }

        // Render children
        UIElement::Render(device, defaultFont);
    }

    std::string UITextBox::GetDisplayText() const
    {
        if (m_passwordMode) {
            return std::string(m_text.length(), '*');
        }
        return m_text;
    }

    size_t UITextBox::GetCharIndexAtPosition(float localX) const
    {
        Font* fontToUse = m_font ? m_font : Engine::Get()->GetUI()->GetDefaultFont();
        if (!fontToUse) return 0;

        std::string display = GetDisplayText();
        float x = 5; // padding
        for (size_t i = 0; i <= display.length(); ++i) {
            if (i == display.length()) return i;
            std::string prefix = display.substr(0, i+1);
            float width = fontToUse->GetTextWidth(prefix);
            if (x + width > localX) {
                // Check which half of the character we're in
                float charWidth = (i > 0) ? fontToUse->GetTextWidth(display.substr(i,1)) : width;
                if (localX - x < charWidth * 0.5f) {
                    return i;
                } else {
                    return i+1;
                }
            }
            x += width;
        }
        return display.length();
    }

} // namespace USE