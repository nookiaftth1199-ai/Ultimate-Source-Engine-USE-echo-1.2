// ============================================================
// Ultimate Source Engine - UI Checkbox Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UICheckbox.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"

namespace USE {

    UICheckbox::UICheckbox()
        : m_checked(false)
        , m_labelFont(nullptr)
        , m_hovered(false)
        , m_boxColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_boxHoverColor(0.3f, 0.3f, 0.3f, 1.0f)
        , m_boxCheckedColor(0.4f, 0.6f, 1.0f, 1.0f)
        , m_checkColor(0.0f, 0.0f, 0.0f, 1.0f)
        , m_labelColor(1.0f, 1.0f, 1.0f, 1.0f)
    {
        // Default size: 20x20 box plus some space for label
        SetSize(20.0f, 20.0f);
    }

    void UICheckbox::SetChecked(bool checked)
    {
        if (m_checked != checked) {
            m_checked = checked;
            if (m_onChanged) {
                m_onChanged(m_checked);
            }
        }
    }

    void UICheckbox::GetBoxRect(float& left, float& top, float& right, float& bottom) const
    {
        // The box is a square at the left side of the element
        float boxSize = m_height; // assume height == box size
        left = m_x;
        top = m_y;
        right = m_x + boxSize;
        bottom = m_y + boxSize;
    }

    bool UICheckbox::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;

        float l, t, r, b;
        GetBoxRect(l, t, r, b);
        m_hovered = (x >= l && x <= r && y >= t && y <= b);
        return m_hovered;
    }

    bool UICheckbox::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false;

        float l, t, r, b;
        GetBoxRect(l, t, r, b);
        if (down && x >= l && x <= r && y >= t && y <= b) {
            // Toggle state
            SetChecked(!m_checked);
            return true;
        }
        return false;
    }

    void UICheckbox::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        float l, t, r, b;
        GetBoxRect(l, t, r, b);

        // Determine box color
        Color boxColor;
        if (m_checked) {
            boxColor = m_boxCheckedColor;
        } else if (m_hovered) {
            boxColor = m_boxHoverColor;
        } else {
            boxColor = m_boxColor;
        }

        // Draw box
        glBegin(GL_QUADS);
        glColor4f(boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        glVertex2f(l, t);
        glVertex2f(r, t);
        glVertex2f(r, b);
        glVertex2f(l, b);
        glEnd();

        // Draw checkmark if checked
        if (m_checked) {
            // Simple X mark (could be a tick)
            glBegin(GL_LINES);
            glColor4f(m_checkColor.r, m_checkColor.g, m_checkColor.b, m_checkColor.a);
            glVertex2f(l + 2, t + 2);
            glVertex2f(r - 2, b - 2);
            glVertex2f(l + 2, b - 2);
            glVertex2f(r - 2, t + 2);
            glEnd();
        }

        // Draw label if present
        if (!m_label.empty()) {
            Font* fontToUse = m_labelFont ? m_labelFont : defaultFont;
            if (fontToUse) {
                float labelX = r + 5;
                float labelY = t + (b - t - fontToUse->GetLineHeight()) * 0.5f;
                fontToUse->RenderText(static_cast<int>(labelX), static_cast<int>(labelY),
                                       m_label, m_labelColor);
            }
        }

        // Render children
        UIElement::Render(device, defaultFont);
    }

} // namespace USE