// ============================================================
// Ultimate Source Engine - UI Label Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UILabel.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"

namespace USE {

    UILabel::UILabel()
        : m_font(nullptr)
        , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_alignment(0)
    {
    }

    void UILabel::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        Font* fontToUse = m_font ? m_font : defaultFont;
        if (fontToUse && !m_text.empty()) {
            float textWidth = fontToUse->GetTextWidth(m_text);
            float textHeight = fontToUse->GetLineHeight();

            float textX = m_x;
            if (m_alignment == 1) {
                textX += (m_width - textWidth) * 0.5f;
            } else if (m_alignment == 2) {
                textX += m_width - textWidth;
            }

            float textY = m_y + (m_height - textHeight) * 0.5f;

            fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY),
                                   m_text, m_textColor);
        }

        // Render children (if any)
        UIElement::Render(device, defaultFont);
    }

} // namespace USE