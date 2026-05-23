// ============================================================
// Ultimate Source Engine - UI Container
//============================================================
//
// A container that arranges its children according to a layout
// (vertical stack, horizontal stack). Supports margins, padding,
// and alignment.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"

namespace USE {

    enum class LayoutDirection {
        Vertical,
        Horizontal
    };

    enum class Alignment {
        Start,
        Center,
        End,
        Stretch
    };

    class UIContainer : public UIElement {
    public:
        UIContainer();
        virtual ~UIContainer() = default;

        // Layout properties
        void SetLayoutDirection(LayoutDirection direction) { m_direction = direction; }
        LayoutDirection GetLayoutDirection() const { return m_direction; }

        void SetSpacing(float spacing) { m_spacing = spacing; }
        float GetSpacing() const { return m_spacing; }

        void SetMargins(float left, float right, float top, float bottom);
        float GetMarginLeft() const { return m_marginLeft; }
        float GetMarginRight() const { return m_marginRight; }
        float GetMarginTop() const { return m_marginTop; }
        float GetMarginBottom() const { return m_marginBottom; }

        void SetPadding(float left, float right, float top, float bottom);
        float GetPaddingLeft() const { return m_paddingLeft; }
        float GetPaddingRight() const { return m_paddingRight; }
        float GetPaddingTop() const { return m_paddingTop; }
        float GetPaddingBottom() const { return m_paddingBottom; }

        // Alignment of children within the container
        void SetAlignment(Alignment alignment) { m_alignment = alignment; }
        Alignment GetAlignment() const { return m_alignment; }

        // Override Update to reposition children
        void Update(float deltaTime) override;

    protected:
        // Apply layout to children
        void UpdateLayout();

    private:
        LayoutDirection m_direction;
        float m_spacing;
        float m_marginLeft, m_marginRight, m_marginTop, m_marginBottom;
        float m_paddingLeft, m_paddingRight, m_paddingTop, m_paddingBottom;
        Alignment m_alignment;
    };

} // namespace USE