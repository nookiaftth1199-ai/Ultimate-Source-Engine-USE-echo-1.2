// ============================================================
// Ultimate Source Engine - UI Progress Bar Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIProgressBar.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"
#include <sstream>
#include <iomanip>

namespace USE {

    UIProgressBar::UIProgressBar()
        : m_min(0.0f)
        , m_max(1.0f)
        , m_value(0.5f)
        , m_horizontal(true)
        , m_bgColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_fillColor(0.3f, 0.6f, 1.0f, 1.0f)
        , m_showPercentage(false)
        , m_font(nullptr)
        , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
    {
    }

    void UIProgressBar::SetRange(float min, float max)
    {
        m_min = min;
        m_max = max;
        SetValue(m_value); // clamp
    }

    void UIProgressBar::SetValue(float value)
    {
        m_value = MathUtils::Clamp(value, m_min, m_max);
    }

    void UIProgressBar::GetFillRect(float& x, float& y, float& w, float& h) const
    {
        float t = (m_value - m_min) / (m_max - m_min);
        t = MathUtils::Clamp(t, 0.0f, 1.0f);

        if (m_horizontal) {
            x = m_x;
            y = m_y;
            w = m_width * t;
            h = m_height;
        } else {
            x = m_x;
            y = m_y + m_height * (1.0f - t); // fill from bottom
            w = m_width;
            h = m_height * t;
        }
    }

    void UIProgressBar::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        // Draw background
        glBegin(GL_QUADS);
        glColor4f(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Draw fill
        float fx, fy, fw, fh;
        GetFillRect(fx, fy, fw, fh);
        glBegin(GL_QUADS);
        glColor4f(m_fillColor.r, m_fillColor.g, m_fillColor.b, m_fillColor.a);
        glVertex2f(fx, fy);
        glVertex2f(fx + fw, fy);
        glVertex2f(fx + fw, fy + fh);
        glVertex2f(fx, fy + fh);
        glEnd();

        // Draw percentage text if enabled
        if (m_showPercentage) {
            Font* fontToUse = m_font ? m_font : defaultFont;
            if (fontToUse) {
                float percentage = ((m_value - m_min) / (m_max - m_min)) * 100.0f;
                std::stringstream ss;
                ss << std::fixed << std::setprecision(1) << percentage << "%";
                std::string text = ss.str();

                // Center text
                float textWidth = fontToUse->GetTextWidth(text);
                float textHeight = fontToUse->GetLineHeight();
                float textX = m_x + (m_width - textWidth) * 0.5f;
                float textY = m_y + (m_height - textHeight) * 0.5f;

                fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY),
                                       text, m_textColor);
            }
        }

        // Render children
        UIElement::Render(device, defaultFont);
    }

} // namespace USE