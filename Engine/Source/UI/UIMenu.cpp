// ============================================================
// Ultimate Source Engine - UI Menu Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIMenu.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include <algorithm>

namespace USE {

    UIMenu::UIMenu()
        : m_font(nullptr)
        , m_bgColor(0.1f, 0.1f, 0.1f, 0.9f)
        , m_titleColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_selectedIndex(-1)
    {
    }

    void UIMenu::AddItem(UIElement* item, bool selectable)
    {
        // Take ownership (UIElement's children are unique_ptrs)
        AddChild(item);
        if (selectable) {
            m_selectableIndices.push_back(m_children.size() - 1);
        }
        UpdateLayout();
    }

    void UIMenu::ClearItems()
    {
        m_children.clear();
        m_selectableIndices.clear();
        m_selectedIndex = -1;
    }

    void UIMenu::SetSelectedIndex(int index)
    {
        if (m_selectableIndices.empty()) {
            m_selectedIndex = -1;
            return;
        }
        if (index < 0) index = 0;
        if (index >= (int)m_selectableIndices.size())
            index = (int)m_selectableIndices.size() - 1;
        m_selectedIndex = index;

        // Optionally set focus on the actual child element
        if (m_selectedIndex >= 0) {
            UIElement* item = GetSelectedItem();
            // You could call a method to highlight the item here
        }
    }

    UIElement* UIMenu::GetSelectedItem() const
    {
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_selectableIndices.size()) {
            size_t childIdx = m_selectableIndices[m_selectedIndex];
            if (childIdx < m_children.size())
                return m_children[childIdx].get();
        }
        return nullptr;
    }

    bool UIMenu::OnKeyPress(int key, bool down)
    {
        if (!m_visible || !m_enabled || !down) return false;

        // Navigation keys
        if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_RETURN || key == SDLK_SPACE) {
            if (m_selectableIndices.empty()) return false;

            if (key == SDLK_UP) {
                SetSelectedIndex(m_selectedIndex - 1);
                return true;
            }
            if (key == SDLK_DOWN) {
                SetSelectedIndex(m_selectedIndex + 1);
                return true;
            }
            if (key == SDLK_RETURN || key == SDLK_SPACE) {
                if (m_selectedIndex >= 0) {
                    UIElement* item = GetSelectedItem();
                    if (item && m_onActivated) {
                        m_onActivated(m_selectedIndex, item);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    void UIMenu::Update(float deltaTime)
    {
        // Update all children (positions may have changed)
        for (auto& child : m_children) {
            child->Update(deltaTime);
        }
    }

    void UIMenu::UpdateLayout()
    {
        // Simple vertical layout: stack items from top with spacing
        float y = m_y + (m_title.empty() ? 0 : (m_font ? m_font->GetLineHeight() + 10 : 30));
        float spacing = 5.0f;
        for (auto& child : m_children) {
            if (child->IsVisible()) {
                child->SetPosition(m_x + (m_width - child->GetWidth()) * 0.5f, y);
                y += child->GetHeight() + spacing;
            }
        }
    }

    void UIMenu::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        Font* fontToUse = m_font ? m_font : defaultFont;

        // Draw background
        glBegin(GL_QUADS);
        glColor4f(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Draw title
        if (!m_title.empty() && fontToUse) {
            float textX = m_x + (m_width - fontToUse->GetTextWidth(m_title)) * 0.5f;
            float textY = m_y + 10;
            fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY),
                                   m_title, m_titleColor);
        }

        // Render children (menu items)
        for (auto& child : m_children) {
            child->Render(device, defaultFont);
        }

        // Optionally draw a highlight around selected item
        UIElement* selected = GetSelectedItem();
        if (selected) {
            // Draw a simple outline
            glBegin(GL_LINE_LOOP);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glVertex2f(selected->GetX() - 2, selected->GetY() - 2);
            glVertex2f(selected->GetX() + selected->GetWidth() + 2, selected->GetY() - 2);
            glVertex2f(selected->GetX() + selected->GetWidth() + 2, selected->GetY() + selected->GetHeight() + 2);
            glVertex2f(selected->GetX() - 2, selected->GetY() + selected->GetHeight() + 2);
            glEnd();
        }

        // Note: UIElement::Render would also render children, but we already did.
        // To avoid double rendering, we don't call the base Render.
    }

} // namespace USE