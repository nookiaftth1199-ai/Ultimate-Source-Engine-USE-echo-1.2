// ============================================================
// Ultimate Source Engine - UI Button
//============================================================
//
// A clickable button with text. Supports hover and pressed states,
// and fires a callback when clicked.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <functional>

namespace USE {

    class Font;

    class UIButton : public UIElement {
    public:
        UIButton();
        virtual ~UIButton() = default;

        // Text
        void SetText(const std::string& text) { m_text = text; }
        const std::string& GetText() const { return m_text; }

        // Font
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Colors for different states
        void SetNormalColor(const Color& color) { m_normalColor = color; }
        void SetHoverColor(const Color& color) { m_hoverColor = color; }
        void SetPressedColor(const Color& color) { m_pressedColor = color; }
        void SetTextColor(const Color& color) { m_textColor = color; }

        // Click callback
        void SetOnClick(std::function<void()> callback) { m_onClick = callback; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        enum class State { Normal, Hover, Pressed };
        State m_state;

        std::string m_text;
        Font*       m_font; // optional, if null use default font

        Color m_normalColor;
        Color m_hoverColor;
        Color m_pressedColor;
        Color m_textColor;

        std::function<void()> m_onClick;
    };

} // namespace USE#pragma