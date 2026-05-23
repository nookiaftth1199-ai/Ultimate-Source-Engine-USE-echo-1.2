// ============================================================
// Ultimate Source Engine - UI Style
//============================================================
//
// Defines a collection of visual properties (colors, fonts, margins,
// padding, etc.) that can be applied to UI elements for consistent
// theming across the application.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include <memory>

namespace USE {

    class Font;

    class UIStyle {
    public:
        UIStyle();
        explicit UIStyle(const std::string& name);
        ~UIStyle() = default;

        // Name for identification
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        // Font
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Text color
        void SetTextColor(const Color& color) { m_textColor = color; }
        const Color& GetTextColor() const { return m_textColor; }

        // Background colors for different states
        void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
        const Color& GetBackgroundColor() const { return m_backgroundColor; }

        void SetHoverBackgroundColor(const Color& color) { m_hoverBackgroundColor = color; }
        const Color& GetHoverBackgroundColor() const { return m_hoverBackgroundColor; }

        void SetPressedBackgroundColor(const Color& color) { m_pressedBackgroundColor = color; }
        const Color& GetPressedBackgroundColor() const { return m_pressedBackgroundColor; }

        void SetSelectedBackgroundColor(const Color& color) { m_selectedBackgroundColor = color; }
        const Color& GetSelectedBackgroundColor() const { return m_selectedBackgroundColor; }

        // Border colors (optional)
        void SetBorderColor(const Color& color) { m_borderColor = color; }
        const Color& GetBorderColor() const { return m_borderColor; }

        void SetBorderWidth(float width) { m_borderWidth = width; }
        float GetBorderWidth() const { return m_borderWidth; }

        // Margins and padding (for containers)
        void SetMargins(float left, float right, float top, float bottom);
        float GetMarginLeft() const { return m_marginLeft; }
        float GetMarginRight() const { return m_marginRight; }
        float GetMarginTop() const { return m_marginTop; }
        float GetMarginBottom() const { return m_marginBottom; }

        void SetPadding(float left, float right, float top, float bottom);
        float GetPaddingLeft() const { return m_paddingLeft; }
        float GetPaddingRight() const { return m_paddingRight; }
        float GetPaddingTop() const { return m_paddingTop; }
        float GetPaddingBottom() const { return m_paddingBottom; }

        // Default style (global)
        static UIStyle* GetDefaultStyle();
        static void SetDefaultStyle(UIStyle* style); // transfers ownership

    private:
        std::string m_name;
        Font* m_font; // not owned

        Color m_textColor;
        Color m_backgroundColor;
        Color m_hoverBackgroundColor;
        Color m_pressedBackgroundColor;
        Color m_selectedBackgroundColor;
        Color m_borderColor;
        float m_borderWidth;

        float m_marginLeft, m_marginRight, m_marginTop, m_marginBottom;
        float m_paddingLeft, m_paddingRight, m_paddingTop, m_paddingBottom;

        static std::unique_ptr<UIStyle> s_defaultStyle;
    };

} // namespace USE