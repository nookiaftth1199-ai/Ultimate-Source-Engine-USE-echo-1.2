// ============================================================
// Ultimate Source Engine - UI Checkbox
//============================================================
//
// A checkbox that can be toggled on/off. Displays a box with
// an optional label and a checkmark when checked.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <functional>
#include <string>

namespace USE {

    class Font;

    class UICheckbox : public UIElement {
    public:
        UICheckbox();
        virtual ~UICheckbox() = default;

        // Checked state
        void SetChecked(bool checked);
        bool IsChecked() const { return m_checked; }

        // Optional label
        void SetLabel(const std::string& label) { m_label = label; }
        const std::string& GetLabel() const { return m_label; }

        // Colors
        void SetBoxColor(const Color& color) { m_boxColor = color; }
        void SetBoxHoverColor(const Color& color) { m_boxHoverColor = color; }
        void SetBoxCheckedColor(const Color& color) { m_boxCheckedColor = color; }
        void SetCheckColor(const Color& color) { m_checkColor = color; }
        void SetLabelColor(const Color& color) { m_labelColor = color; }

        // Callback when state changes
        void SetOnChanged(std::function<void(bool)> callback) { m_onChanged = callback; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        bool        m_checked;
        std::string m_label;
        Font*       m_labelFont; // optional, if null use default font

        bool        m_hovered;    // mouse over the checkbox box

        Color m_boxColor;
        Color m_boxHoverColor;
        Color m_boxCheckedColor;
        Color m_checkColor;
        Color m_labelColor;

        std::function<void(bool)> m_onChanged;

        // Helper to get the box rectangle (square part)
        void GetBoxRect(float& left, float& top, float& right, float& bottom) const;
    };

} // namespace USE