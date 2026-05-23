// ============================================================
// Ultimate Source Engine - UI Slider Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UISlider.h"
#include "Renderer/IRenderDevice.h"
#include "Math/MathUtils.h"
#include <algorithm>

namespace USE {

    UISlider::UISlider()
        : m_min(0.0f)
        , m_max(1.0f)
        , m_value(0.5f)
        , m_step(0.0f)
        , m_orientation(SliderOrientation::Horizontal)
        , m_thumbWidth(20.0f)
        , m_thumbHeight(20.0f)
        , m_trackColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_filledTrackColor(0.6f, 0.6f, 0.6f, 1.0f)
        , m_thumbColor(0.8f, 0.8f, 0.8f, 1.0f)
        , m_thumbHoverColor(0.9f, 0.9f, 0.9f, 1.0f)
        , m_thumbDraggedColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_dragging(false)
        , m_thumbHovered(false)
    {
    }

    void UISlider::SetRange(float min, float max)
    {
        m_min = min;
        m_max = max;
        SetValue(m_value); // clamp
    }

    void UISlider::SetValue(float value)
    {
        float newValue = MathUtils::Clamp(value, m_min, m_max);
        if (m_step > 0.0f) {
            // Snap to nearest step
            float steps = roundf((newValue - m_min) / m_step);
            newValue = m_min + steps * m_step;
        }
        if (newValue != m_value) {
            m_value = newValue;
            if (m_onValueChanged) {
                m_onValueChanged(m_value);
            }
        }
    }

    void UISlider::SetThumbSize(float width, float height)
    {
        m_thumbWidth = width;
        m_thumbHeight = height;
    }

    void UISlider::GetThumbRect(float& left, float& top, float& right, float& bottom) const
    {
        float t = (m_value - m_min) / (m_max - m_min); // 0..1
        if (m_orientation == SliderOrientation::Horizontal) {
            left = m_x + t * (m_width - m_thumbWidth);
            top = m_y + (m_height - m_thumbHeight) * 0.5f;
            right = left + m_thumbWidth;
            bottom = top + m_thumbHeight;
        } else {
            // Vertical: value increases upward? Usually bottom to top.
            // We'll define that value increases as thumb moves up.
            // So t=0 at bottom, t=1 at top.
            top = m_y + (1.0f - t) * (m_height - m_thumbHeight);
            left = m_x + (m_width - m_thumbWidth) * 0.5f;
            right = left + m_thumbWidth;
            bottom = top + m_thumbHeight;
        }
    }

    void UISlider::UpdateValueFromMouse(float mouseX, float mouseY)
    {
        float t;
        if (m_orientation == SliderOrientation::Horizontal) {
            float trackX = m_x + m_thumbWidth * 0.5f;
            float trackWidth = m_width - m_thumbWidth;
            if (trackWidth <= 0) t = 0.5f;
            else {
                t = (mouseX - trackX) / trackWidth;
                t = MathUtils::Clamp(t, 0.0f, 1.0f);
            }
        } else {
            float trackY = m_y;
            float trackHeight = m_height - m_thumbHeight;
            if (trackHeight <= 0) t = 0.5f;
            else {
                t = (mouseY - trackY) / trackHeight;
                t = 1.0f - MathUtils::Clamp(t, 0.0f, 1.0f); // invert because y increases downward
            }
        }
        float newValue = m_min + t * (m_max - m_min);
        SetValue(newValue);
    }

    bool UISlider::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;

        if (m_dragging) {
            UpdateValueFromMouse(x, y);
            return true;
        } else {
            // Check if mouse is over thumb
            float l, t, r, b;
            GetThumbRect(l, t, r, b);
            bool overThumb = (x >= l && x <= r && y >= t && y <= b);
            m_thumbHovered = overThumb;
            return overThumb; // we consume hover to prevent underlying elements
        }
    }

    bool UISlider::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false;

        if (down) {
            // Check if click is on thumb
            float l, t, r, b;
            GetThumbRect(l, t, r, b);
            if (x >= l && x <= r && y >= t && y <= b) {
                m_dragging = true;
                return true;
            } else if (ContainsPoint(x, y)) {
                // Click on track – jump thumb to that position
                UpdateValueFromMouse(x, y);
                m_dragging = true;
                return true;
            }
        } else {
            if (m_dragging) {
                m_dragging = false;
                return true;
            }
        }
        return false;
    }

    void UISlider::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        // Draw track
        if (m_orientation == SliderOrientation::Horizontal) {
            float trackX = m_x + m_thumbWidth * 0.5f;
            float trackY = m_y + m_height * 0.5f - 2;
            float trackWidth = m_width - m_thumbWidth;
            float trackHeight = 4;

            glBegin(GL_QUADS);
            glColor4f(m_trackColor.r, m_trackColor.g, m_trackColor.b, m_trackColor.a);
            glVertex2f(trackX, trackY);
            glVertex2f(trackX + trackWidth, trackY);
            glVertex2f(trackX + trackWidth, trackY + trackHeight);
            glVertex2f(trackX, trackY + trackHeight);
            glEnd();

            // Filled portion
            float t = (m_value - m_min) / (m_max - m_min);
            float fillWidth = t * trackWidth;
            glBegin(GL_QUADS);
            glColor4f(m_filledTrackColor.r, m_filledTrackColor.g, m_filledTrackColor.b, m_filledTrackColor.a);
            glVertex2f(trackX, trackY);
            glVertex2f(trackX + fillWidth, trackY);
            glVertex2f(trackX + fillWidth, trackY + trackHeight);
            glVertex2f(trackX, trackY + trackHeight);
            glEnd();
        } else {
            // Vertical
            float trackX = m_x + m_width * 0.5f - 2;
            float trackY = m_y;
            float trackWidth = 4;
            float trackHeight = m_height - m_thumbHeight;

            glBegin(GL_QUADS);
            glColor4f(m_trackColor.r, m_trackColor.g, m_trackColor.b, m_trackColor.a);
            glVertex2f(trackX, trackY);
            glVertex2f(trackX + trackWidth, trackY);
            glVertex2f(trackX + trackWidth, trackY + trackHeight);
            glVertex2f(trackX, trackY + trackHeight);
            glEnd();

            // Filled portion (from bottom)
            float t = (m_value - m_min) / (m_max - m_min);
            float fillHeight = t * trackHeight;
            glBegin(GL_QUADS);
            glColor4f(m_filledTrackColor.r, m_filledTrackColor.g, m_filledTrackColor.b, m_filledTrackColor.a);
            glVertex2f(trackX, trackY + trackHeight - fillHeight);
            glVertex2f(trackX + trackWidth, trackY + trackHeight - fillHeight);
            glVertex2f(trackX + trackWidth, trackY + trackHeight);
            glVertex2f(trackX, trackY + trackHeight);
            glEnd();
        }

        // Draw thumb
        float l, t, r, b;
        GetThumbRect(l, t, r, b);

        Color thumbColor = m_thumbColor;
        if (m_dragging) {
            thumbColor = m_thumbDraggedColor;
        } else if (m_thumbHovered) {
            thumbColor = m_thumbHoverColor;
        }

        glBegin(GL_QUADS);
        glColor4f(thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
        glVertex2f(l, t);
        glVertex2f(r, t);
        glVertex2f(r, b);
        glVertex2f(l, b);
        glEnd();

        // Render children
        UIElement::Render(device, defaultFont);
    }

} // namespace USE