// ============================================================
// Ultimate Source Engine - UI Button Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIButton.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"

namespace USE {

    UIButton::UIButton()
        : m_state(State::Normal)
        , m_font(nullptr)
        , m_normalColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_hoverColor(0.3f, 0.3f, 0.3f, 1.0f)
        , m_pressedColor(0.1f, 0.1f, 0.1f, 1.0f)
        , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
    {
    }

    bool UIButton::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;

        bool inside = ContainsPoint(x, y);
        if (inside) {
            m_state = State::Hover;
            return true;
        } else {
            if (m_state != State::Pressed) // don't change if pressed
                m_state = State::Normal;
            return false;
        }
    }

    bool UIButton::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false; // left button only

        bool inside = ContainsPoint(x, y);
        if (down) {
            if (inside) {
                m_state = State::Pressed;
                return true;
            }
        } else {
            if (m_state == State::Pressed && inside) {
                // Click!
                if (m_onClick) m_onClick();
                m_state = State::Hover;
                return true;
            } else {
                m_state = (inside ? State::Hover : State::Normal);
            }
        }
        return false;
    }

    void UIButton::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        // Determine current color based on state
        Color bgColor;
        switch (m_state) {
            case State::Normal:  bgColor = m_normalColor; break;
            case State::Hover:   bgColor = m_hoverColor; break;
            case State::Pressed: bgColor = m_pressedColor; break;
            default:             bgColor = m_normalColor; break;
        }

        // Draw background (simple quad)
        // For a real implementation, you'd use a 2D renderer.
        // Here we use immediate mode OpenGL as placeholder.
        glBegin(GL_QUADS);
        glColor4f(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Draw text
        Font* fontToUse = m_font ? m_font : defaultFont;
        if (fontToUse && !m_text.empty()) {
            // Center text
            float textWidth = fontToUse->GetTextWidth(m_text);
            float textHeight = fontToUse->GetLineHeight();
            float textX = m_x + (m_width - textWidth) * 0.5f;
            float textY = m_y + (m_height - textHeight) * 0.5f;
            fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY),
                                   m_text, m_textColor);
        }

        // Render children
        UIElement::Render(device, defaultFont);
    }

} // namespace USE