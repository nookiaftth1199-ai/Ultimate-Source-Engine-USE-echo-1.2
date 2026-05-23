// ============================================================
// Ultimate Source Engine - UI Progress Bar
//============================================================
//
// A progress bar that displays a value between min and max.
// Supports horizontal and vertical orientation, custom colors,
// and optional percentage text.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <functional>

namespace USE {

    class Font;

    class UIProgressBar : public UIElement {
    public:
        UIProgressBar();
        virtual ~UIProgressBar() = default;

        // Range and value
        void SetRange(float min, float max);
        float GetMin() const { return m_min; }
        float GetMax() const { return m_max; }

        void SetValue(float value);
        float GetValue() const { return m_value; }

        // Orientation
        void SetOrientation(bool horizontal) { m_horizontal = horizontal; }
        bool IsHorizontal() const { return m_horizontal; }

        // Colors
        void SetBackgroundColor(const Color& color) { m_bgColor = color; }
        void SetFillColor(const Color& color) { m_fillColor = color; }

        // Optional percentage text
        void SetShowPercentage(bool show) { m_showPercentage = show; }
        bool GetShowPercentage() const { return m_showPercentage; }
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }
        void SetTextColor(const Color& color) { m_textColor = color; }

        // Overrides
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        float m_min;
        float m_max;
        float m_value;
        bool  m_horizontal;

        Color m_bgColor;
        Color m_fillColor;

        bool  m_showPercentage;
        Font* m_font;
        Color m_textColor;

        // Helper to get fill rectangle
        void GetFillRect(float& x, float& y, float& w, float& h) const;
    };

} // namespace USE