// ============================================================
// Ultimate Source Engine - UI Text Box
//============================================================
//
// A text input box that allows the user to enter and edit text.
// Supports cursor movement, selection, clipboard operations,
// and password masking.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <string>

namespace USE {

    class Font;

    class UITextBox : public UIElement {
    public:
        UITextBox();
        virtual ~UITextBox() = default;

        // Text content
        void SetText(const std::string& text);
        const std::string& GetText() const { return m_text; }

        // Placeholder text (shown when empty)
        void SetPlaceholder(const std::string& text) { m_placeholder = text; }
        const std::string& GetPlaceholder() const { return m_placeholder; }

        // Font
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Colors
        void SetTextColor(const Color& color) { m_textColor = color; }
        void SetPlaceholderColor(const Color& color) { m_placeholderColor = color; }
        void SetBackgroundColor(const Color& color) { m_bgColor = color; }
        void SetCursorColor(const Color& color) { m_cursorColor = color; }
        void SetSelectionColor(const Color& color) { m_selectionColor = color; }

        // Max length (0 = unlimited)
        void SetMaxLength(size_t maxLength) { m_maxLength = maxLength; }
        size_t GetMaxLength() const { return m_maxLength; }

        // Password mode (display '*' instead of actual characters)
        void SetPasswordMode(bool enable) { m_passwordMode = enable; }
        bool IsPasswordMode() const { return m_passwordMode; }

        // Read-only mode (cannot edit)
        void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }
        bool IsReadOnly() const { return m_readOnly; }

        // Overrides
        bool OnMouseButton(int button, bool down, float x, float y) override;
        bool OnKeyPress(int key, bool down) override;
        bool OnCharInput(unsigned int codepoint) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        std::string m_text;
        std::string m_placeholder;
        Font*       m_font;

        Color m_textColor;
        Color m_placeholderColor;
        Color m_bgColor;
        Color m_cursorColor;
        Color m_selectionColor;

        size_t m_maxLength;
        bool   m_passwordMode;
        bool   m_readOnly;

        // Editing state
        size_t m_cursorPos;          // index between characters (0 = before first)
        size_t m_selectionStart;     // start of selection (cursor pos if no selection)
        size_t m_selectionEnd;       // end of selection
        float  m_cursorBlinkTimer;
        bool   m_cursorVisible;

        // Internal helpers
        void InsertCharacter(char c);
        void DeleteCharacter(bool forward);
        void MoveCursor(int direction, bool shiftPressed); // -1 left, +1 right
        void MoveCursorWord(int direction, bool shiftPressed);
        void SelectAll();
        void DeleteSelection();
        std::string GetDisplayText() const; // applies password masking
        size_t GetCharIndexAtPosition(float localX) const; // for mouse click
    };

} // namespace USE