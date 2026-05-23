// ============================================================
// Ultimate Source Engine - UI Label
//============================================================
//
// A simple text label for displaying static or dynamic text.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <string>

namespace USE {

    class Font;

    class UILabel : public UIElement {
    public:
        UILabel();
        virtual ~UILabel() = default;

        // Text
        void SetText(const std::string& text) { m_text = text; }
        const std::string& GetText() const { return m_text; }

        // Font
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Text color
        void SetTextColor(const Color& color) { m_textColor = color; }
        const Color& GetTextColor() const { return m_textColor; }

        // Alignment (0 = left, 1 = center, 2 = right)
        void SetAlignment(int align) { m_alignment = align; }
        int GetAlignment() const { return m_alignment; }

        // Override render
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        std::string m_text;
        Font*       m_font;
        Color       m_textColor;
        int         m_alignment; // 0 left, 1 center, 2 right
    };

} // namespace USE