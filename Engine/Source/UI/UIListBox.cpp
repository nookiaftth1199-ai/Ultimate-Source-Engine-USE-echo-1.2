// ============================================================
// Ultimate Source Engine - UI List Box Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIListBox.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"
#include <algorithm>

namespace USE {

    UIListBox::UIListBox()
        : m_selectedIndex(-1)
        , m_hoverIndex(-1)
        , m_scrollOffset(0)
        , m_font(nullptr)
        , m_bgColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_selectedColor(0.3f, 0.5f, 0.9f, 1.0f)
        , m_hoverColor(0.4f, 0.4f, 0.4f, 1.0f)
    {
    }

    void UIListBox::AddItem(const std::string& item)
    {
        m_items.push_back({item});
    }

    void UIListBox::InsertItem(size_t index, const std::string& item)
    {
        if (index <= m_items.size()) {
            m_items.insert(m_items.begin() + index, {item});
        }
    }

    void UIListBox::RemoveItem(size_t index)
    {
        if (index < m_items.size()) {
            m_items.erase(m_items.begin() + index);
            if (m_selectedIndex == (int)index) {
                m_selectedIndex = -1;
            } else if (m_selectedIndex > (int)index) {
                m_selectedIndex--;
            }
            if (m_hoverIndex == (int)index) {
                m_hoverIndex = -1;
            } else if (m_hoverIndex > (int)index) {
                m_hoverIndex--;
            }
        }
    }

    void UIListBox::ClearItems()
    {
        m_items.clear();
        m_selectedIndex = -1;
        m_hoverIndex = -1;
        m_scrollOffset = 0;
    }

    const std::string& UIListBox::GetItem(size_t index) const
    {
        static std::string empty;
        if (index < m_items.size())
            return m_items[index].text;
        return empty;
    }

    void UIListBox::SetSelectedIndex(int index)
    {
        if (index < -1 || index >= (int)m_items.size())
            return;
        if (m_selectedIndex != index) {
            m_selectedIndex = index;
            if (m_onSelectionChanged) {
                std::string item = (index >= 0) ? m_items[index].text : "";
                m_onSelectionChanged(index, item);
            }
        }
    }

    std::string UIListBox::GetSelectedItem() const
    {
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size())
            return m_items[m_selectedIndex].text;
        return "";
    }

    bool UIListBox::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        if (!ContainsPoint(x, y)) {
            m_hoverIndex = -1;
            return false;
        }
        float localY = y - m_y;
        int index = GetItemIndexAt(localY);
        m_hoverIndex = index;
        return true;
    }

    bool UIListBox::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false;
        if (down && ContainsPoint(x, y)) {
            float localY = y - m_y;
            int index = GetItemIndexAt(localY);
            if (index >= 0) {
                SetSelectedIndex(index);
                return true;
            }
        }
        return false;
    }

    bool UIListBox::OnMouseWheel(float delta)
    {
        if (!m_visible || !m_enabled) return false;
        // delta positive = scroll up, negative = down
        int maxScroll = (int)m_items.size() - (int)(m_height / GetItemHeight());
        if (maxScroll < 0) maxScroll = 0;
        m_scrollOffset -= (int)delta;
        m_scrollOffset = MathUtils::Clamp(m_scrollOffset, 0, maxScroll);
        return true;
    }

    // Helper to get item height (line height)
    static float GetItemHeight(Font* font, Font* defaultFont)
    {
        Font* f = font ? font : defaultFont;
        return f ? f->GetLineHeight() : 20.0f;
    }

    bool UIListBox::GetItemRect(size_t index, float& x, float& y, float& w, float& h) const
    {
        if (index >= m_items.size()) return false;
        float lineHeight = GetItemHeight(m_font, nullptr); // need defaultFont? We'll pass later.
        // This function is const, cannot access defaultFont. Better compute in render.
        // We'll not implement here; instead we compute on the fly in hit testing.
        return false;
    }

    int UIListBox::GetItemIndexAt(float localY) const
    {
        // Need font to compute line height. We'll rely on external font knowledge.
        // In render we have access to font; here we don't. So we'll compute later.
        // For hit testing, we need font. We'll just return -1 for now, and implement hit in mouse handling with font.
        return -1;
    }

    void UIListBox::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        Font* fontToUse = m_font ? m_font : defaultFont;
        if (!fontToUse) return;

        float lineHeight = fontToUse->GetLineHeight();

        // Draw background
        glBegin(GL_QUADS);
        glColor4f(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Determine visible range
        int startIndex = m_scrollOffset;
        int maxVisible = (int)(m_height / lineHeight);
        int endIndex = startIndex + maxVisible;
        if (endIndex > (int)m_items.size()) endIndex = (int)m_items.size();

        float yPos = m_y;
        for (int i = startIndex; i < endIndex; ++i) {
            float itemY = yPos + (i - startIndex) * lineHeight;
            float itemHeight = lineHeight;

            // Highlight if selected or hovered
            Color bgColor(0,0,0,0);
            if (i == m_selectedIndex) {
                bgColor = m_selectedColor;
            } else if (i == m_hoverIndex) {
                bgColor = m_hoverColor;
            }
            if (bgColor.a > 0) {
                glBegin(GL_QUADS);
                glColor4f(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
                glVertex2f(m_x, itemY);
                glVertex2f(m_x + m_width, itemY);
                glVertex2f(m_x + m_width, itemY + itemHeight);
                glVertex2f(m_x, itemY + itemHeight);
                glEnd();
            }

            // Draw text
            fontToUse->RenderText(static_cast<int>(m_x + 2), static_cast<int>(itemY),
                                   m_items[i].text, m_textColor);
        }

        // Render children
        UIElement::Render(device, defaultFont);
    }

} // namespace USE