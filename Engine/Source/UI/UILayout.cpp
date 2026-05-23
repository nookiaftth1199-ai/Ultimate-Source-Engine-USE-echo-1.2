// ============================================================
// Ultimate Source Engine - UI Layout Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UILayout.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include <algorithm>

namespace USE {

    UILayout::UILayout()
        : m_policy(LayoutPolicy::Absolute)
        , m_hSpacing(5.0f)
        , m_vSpacing(5.0f)
        , m_marginLeft(0), m_marginRight(0), m_marginTop(0), m_marginBottom(0)
        , m_alignment(LayoutAlignment::Start)
        , m_gridColumns(1)
        , m_layoutDirty(true)
    {
    }

    void UILayout::SetPolicy(LayoutPolicy policy)
    {
        if (m_policy != policy) {
            m_policy = policy;
            m_layoutDirty = true;
        }
    }

    void UILayout::SetSpacing(float horizontalSpacing, float verticalSpacing)
    {
        m_hSpacing = horizontalSpacing;
        m_vSpacing = verticalSpacing;
        m_layoutDirty = true;
    }

    void UILayout::SetMargins(float left, float right, float top, float bottom)
    {
        m_marginLeft = left;
        m_marginRight = right;
        m_marginTop = top;
        m_marginBottom = bottom;
        m_layoutDirty = true;
    }

    void UILayout::SetGridColumns(int columns)
    {
        m_gridColumns = columns;
        m_layoutDirty = true;
    }

    void UILayout::AddChild(std::unique_ptr<UIElement> child)
    {
        m_children.push_back(std::move(child));
        m_layoutDirty = true;
    }

    UIElement* UILayout::AddChild(UIElement* child)
    {
        std::unique_ptr<UIElement> ptr(child);
        AddChild(std::move(ptr));
        return child;
    }

    void UILayout::RemoveChild(UIElement* child)
    {
        auto it = std::find_if(m_children.begin(), m_children.end(),
                                [child](const std::unique_ptr<UIElement>& ptr) {
                                    return ptr.get() == child;
                                });
        if (it != m_children.end()) {
            m_children.erase(it);
            m_layoutDirty = true;
        }
    }

    void UILayout::ClearChildren()
    {
        m_children.clear();
        m_layoutDirty = true;
    }

    void UILayout::Update(float deltaTime)
    {
        for (auto& child : m_children) {
            child->Update(deltaTime);
        }
    }

    void UILayout::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        // Ensure layout is up‑to‑date
        if (m_layoutDirty) {
            UpdateLayout();
        }

        // Render children
        for (auto& child : m_children) {
            child->Render(device, defaultFont);
        }
    }

    bool UILayout::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        // Forward to children in reverse order (topmost first)
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseMove(x, y))
                return true;
        }
        return false;
    }

    bool UILayout::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseButton(button, down, x, y))
                return true;
        }
        return false;
    }

    bool UILayout::OnKeyPress(int key, bool down)
    {
        if (!m_visible || !m_enabled) return false;
        // Usually only focused element gets keys, but we'll forward to all children
        // (they can decide based on their own focus)
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnKeyPress(key, down))
                return true;
        }
        return false;
    }

    bool UILayout::OnCharInput(unsigned int codepoint)
    {
        if (!m_visible || !m_enabled) return false;
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnCharInput(codepoint))
                return true;
        }
        return false;
    }

    void UILayout::UpdateLayout()
    {
        m_layoutDirty = false;
        switch (m_policy) {
            case LayoutPolicy::Absolute:
                DoAbsoluteLayout();
                break;
            case LayoutPolicy::Horizontal:
                DoHorizontalLayout();
                break;
            case LayoutPolicy::Vertical:
                DoVerticalLayout();
                break;
            case LayoutPolicy::Grid:
                DoGridLayout();
                break;
        }
    }

    void UILayout::DoAbsoluteLayout()
    {
        // Nothing to do – children are placed manually
    }

    void UILayout::DoHorizontalLayout()
    {
        float x = m_x + m_marginLeft;
        float y = m_y + m_marginTop;
        float availableWidth = m_width - m_marginLeft - m_marginRight;
        float availableHeight = m_height - m_marginTop - m_marginBottom;

        // Compute total width occupied by children
        float totalChildWidth = 0.0f;
        int visibleCount = 0;
        for (auto& child : m_children) {
            if (child->IsVisible()) {
                totalChildWidth += child->GetWidth();
                visibleCount++;
            }
        }
        if (visibleCount == 0) return;

        float spacing = (visibleCount > 1) ? m_hSpacing * (visibleCount - 1) : 0;
        float neededWidth = totalChildWidth + spacing;

        // Adjust start X based on alignment
        if (m_alignment == LayoutAlignment::Center) {
            x += (availableWidth - neededWidth) * 0.5f;
        } else if (m_alignment == LayoutAlignment::End) {
            x += availableWidth - neededWidth;
        }

        // Position each child
        for (auto& child : m_children) {
            if (!child->IsVisible()) continue;

            // For stretch alignment, adjust width to fill remaining space proportionally? Not implemented.
            child->SetPosition(x, y);
            x += child->GetWidth() + m_hSpacing;
        }
    }

    void UILayout::DoVerticalLayout()
    {
        float x = m_x + m_marginLeft;
        float y = m_y + m_marginTop;
        float availableWidth = m_width - m_marginLeft - m_marginRight;
        float availableHeight = m_height - m_marginTop - m_marginBottom;

        float totalChildHeight = 0.0f;
        int visibleCount = 0;
        for (auto& child : m_children) {
            if (child->IsVisible()) {
                totalChildHeight += child->GetHeight();
                visibleCount++;
            }
        }
        if (visibleCount == 0) return;

        float spacing = (visibleCount > 1) ? m_vSpacing * (visibleCount - 1) : 0;
        float neededHeight = totalChildHeight + spacing;

        if (m_alignment == LayoutAlignment::Center) {
            y += (availableHeight - neededHeight) * 0.5f;
        } else if (m_alignment == LayoutAlignment::End) {
            y += availableHeight - neededHeight;
        }

        for (auto& child : m_children) {
            if (!child->IsVisible()) continue;

            child->SetPosition(x, y);
            y += child->GetHeight() + m_vSpacing;
        }
    }

    void UILayout::DoGridLayout()
    {
        if (m_gridColumns <= 0) m_gridColumns = 1;
        float x = m_x + m_marginLeft;
        float y = m_y + m_marginTop;
        float availableWidth = m_width - m_marginLeft - m_marginRight;
        float cellWidth = availableWidth / m_gridColumns; // equal width cells

        int col = 0;
        float currentY = y;
        float maxHeightInRow = 0.0f;

        for (auto& child : m_children) {
            if (!child->IsVisible()) continue;

            // Determine cell width (maybe child can have its own width, but we'll place it centered)
            float childWidth = child->GetWidth();
            float childX = x + col * cellWidth + (cellWidth - childWidth) * 0.5f;
            child->SetPosition(childX, currentY);

            // Track max height in this row
            if (child->GetHeight() > maxHeightInRow)
                maxHeightInRow = child->GetHeight();

            col++;
            if (col >= m_gridColumns) {
                col = 0;
                currentY += maxHeightInRow + m_vSpacing;
                maxHeightInRow = 0;
            }
        }
    }

} // namespace USE