// ============================================================
// Ultimate Source Engine - UI Layout Container
//============================================================
//
// A container that automatically positions its children according
// to a configurable layout policy (horizontal, vertical, grid, etc.).
// ============================================================

#pragma once

#include "UIElement.h"
#include <vector>
#include <memory>

namespace USE {

    // Layout policies
    enum class LayoutPolicy {
        Absolute,       // children are positioned manually (no automatic layout)
        Horizontal,     // children arranged left to right
        Vertical,       // children arranged top to bottom
        Grid            // children arranged in a grid (rows/columns)
    };

    // Alignment within available space
    enum class LayoutAlignment {
        Start,
        Center,
        End,
        Stretch
    };

    class UILayout : public UIElement {
    public:
        UILayout();
        virtual ~UILayout() = default;

        // Layout policy and parameters
        void SetPolicy(LayoutPolicy policy);
        LayoutPolicy GetPolicy() const { return m_policy; }

        void SetSpacing(float horizontalSpacing, float verticalSpacing = 0.0f);
        void SetMargins(float left, float right, float top, float bottom);

        void SetAlignment(LayoutAlignment alignment) { m_alignment = alignment; }
        LayoutAlignment GetAlignment() const { return m_alignment; }

        // Grid specific: number of columns (rows are determined automatically)
        void SetGridColumns(int columns);
        int GetGridColumns() const { return m_gridColumns; }

        // Child management (ownership taken)
        void AddChild(std::unique_ptr<UIElement> child);
        UIElement* AddChild(UIElement* child); // convenience, transfers ownership
        void RemoveChild(UIElement* child);
        void ClearChildren();

        const std::vector<std::unique_ptr<UIElement>>& GetChildren() const { return m_children; }

        // Overrides
        void Update(float deltaTime) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

        // Input forwarding
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        bool OnKeyPress(int key, bool down) override;
        bool OnCharInput(unsigned int codepoint) override;

    protected:
        // Recalculate child positions based on current policy.
        // Called automatically when needed (e.g., after policy change or resize).
        void UpdateLayout();

    private:
        LayoutPolicy m_policy;
        float m_hSpacing;
        float m_vSpacing;
        float m_marginLeft, m_marginRight, m_marginTop, m_marginBottom;
        LayoutAlignment m_alignment;
        int m_gridColumns; // for grid layout

        std::vector<std::unique_ptr<UIElement>> m_children;
        bool m_layoutDirty; // flag to trigger layout update before rendering

        void DoAbsoluteLayout();
        void DoHorizontalLayout();
        void DoVerticalLayout();
        void DoGridLayout();
    };

} // namespace USE