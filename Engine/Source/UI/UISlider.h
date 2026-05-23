// ============================================================
// Ultimate Source Engine - UI Slider
//============================================================
//
// A slider control that allows selecting a value from a range
// by dragging a thumb. Supports horizontal and vertical orientation.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <functional>

namespace USE {

    enum class SliderOrientation {
        Horizontal,
        Vertical
    };

    class UISlider : public UIElement {
    public:
        UISlider();
        virtual ~UISlider() = default;

        // Value range
        void SetRange(float min, float max);
        float GetMin() const { return m_min; }
        float GetMax() const { return m_max; }

        // Current value
        void SetValue(float value);
        float GetValue() const { return m_value; }

        // Step size (0 = continuous)
        void SetStep(float step) { m_step = step; }
        float GetStep() const { return m_step; }

        // Orientation
        void SetOrientation(SliderOrientation orientation) { m_orientation = orientation; }
        SliderOrientation GetOrientation() const { return m_orientation; }

        // Thumb size (if not set, defaults to 20x20)
        void SetThumbSize(float width, float height);
        float GetThumbWidth() const { return m_thumbWidth; }
        float GetThumbHeight() const { return m_thumbHeight; }

        // Colors
        void SetTrackColor(const Color& color) { m_trackColor = color; }
        void SetFilledTrackColor(const Color& color) { m_filledTrackColor = color; }
        void SetThumbColor(const Color& color) { m_thumbColor = color; }
        void SetThumbHoverColor(const Color& color) { m_thumbHoverColor = color; }
        void SetThumbDraggedColor(const Color& color) { m_thumbDraggedColor = color; }

        // Callback when value changes
        void SetOnValueChanged(std::function<void(float)> callback) { m_onValueChanged = callback; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        float m_min;
        float m_max;
        float m_value;
        float m_step;

        SliderOrientation m_orientation;

        float m_thumbWidth;
        float m_thumbHeight;

        Color m_trackColor;
        Color m_filledTrackColor;
        Color m_thumbColor;
        Color m_thumbHoverColor;
        Color m_thumbDraggedColor;

        bool  m_dragging;
        bool  m_thumbHovered;

        std::function<void(float)> m_onValueChanged;

        // Calculate thumb position rectangle (relative to this element)
        void GetThumbRect(float& left, float& top, float& right, float& bottom) const;

        // Update value based on mouse position
        void UpdateValueFromMouse(float mouseX, float mouseY);
    };

} // namespace USE