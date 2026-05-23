// ============================================================
// Ultimate Source Engine - UI Container Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIContainer.h"

namespace USE {

    UIContainer::UIContainer()
        : m_direction(LayoutDirection::Vertical)
        , m_spacing(5.0f)
        , m_marginLeft(0), m_marginRight(0), m_marginTop(0), m_marginBottom(0)
        , m_paddingLeft(0), m_paddingRight(0), m_paddingTop(0), m_paddingBottom(0)
        , m_alignment(Alignment::Start)
    {
    }

    void UIContainer::SetMargins(float left, float right, float top, float bottom)
    {
        m_marginLeft = left;
        m_marginRight = right;
        m_marginTop = top;
        m_marginBottom = bottom;
    }

    void UIContainer::SetPadding(float left, float right, float top, float bottom)
    {
        m_paddingLeft = left;
        m_paddingRight = right;
        m_paddingTop = top;
        m_paddingBottom = bottom;
    }

    void UIContainer::Update(float deltaTime)
    {
        UpdateLayout();
        UIElement::Update(deltaTime); // let children update
    }

    void UIContainer::UpdateLayout()
    {
        // Available area inside padding
        float contentX = m_x + m_paddingLeft;
        float contentY = m_y + m_paddingTop;
        float contentWidth = m_width - m_paddingLeft - m_paddingRight;
        float contentHeight = m_height - m_paddingTop - m_paddingBottom;

        // If we have no children, nothing to do
        if (m_children.empty()) return;

        if (m_direction == LayoutDirection::Vertical) {
            // Compute total height used by children
            float totalChildHeight = 0.0f;
            for (size_t i = 0; i < m_children.size(); ++i) {
                totalChildHeight += m_children[i]->GetHeight();
                if (i < m_children.size() - 1) totalChildHeight += m_spacing;
            }

            // Starting Y position based on alignment
            float startY = contentY;
            if (m_alignment == Alignment::Center) {
                startY += (contentHeight - totalChildHeight) * 0.5f;
            } else if (m_alignment == Alignment::End) {
                startY += contentHeight - totalChildHeight;
            }

            float currentY = startY;
            for (size_t i = 0; i < m_children.size(); ++i) {
                auto& child = m_children[i];
                // X position based on alignment (horizontal alignment within container)
                float childX = contentX;
                float childWidth = child->GetWidth();
                if (m_alignment == Alignment::Center) {
                    childX += (contentWidth - childWidth) * 0.5f;
                } else if (m_alignment == Alignment::End) {
                    childX += contentWidth - childWidth;
                }
                child->SetPosition(childX, currentY);
                currentY += child->GetHeight() + m_spacing;
            }
        }
        else { // Horizontal
            float totalChildWidth = 0.0f;
            for (size_t i = 0; i < m_children.size(); ++i) {
                totalChildWidth += m_children[i]->GetWidth();
                if (i < m_children.size() - 1) totalChildWidth += m_spacing;
            }

            float startX = contentX;
            if (m_alignment == Alignment::Center) {
                startX += (contentWidth - totalChildWidth) * 0.5f;
            } else if (m_alignment == Alignment::End) {
                startX += contentWidth - totalChildWidth;
            }

            float currentX = startX;
            for (size_t i = 0; i < m_children.size(); ++i) {
                auto& child = m_children[i];
                float childY = contentY;
                float childHeight = child->GetHeight();
                if (m_alignment == Alignment::Center) {
                    childY += (contentHeight - childHeight) * 0.5f;
                } else if (m_alignment == Alignment::End) {
                    childY += contentHeight - childHeight;
                }
                child->SetPosition(currentX, childY);
                currentX += child->GetWidth() + m_spacing;
            }
        }
    }

} // namespace USE