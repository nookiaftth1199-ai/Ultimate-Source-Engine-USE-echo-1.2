// ============================================================
// Ultimate Source Engine - UI Style Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIStyle.h"

namespace USE {

    std::unique_ptr<UIStyle> UIStyle::s_defaultStyle = nullptr;

    UIStyle::UIStyle()
        : m_font(nullptr)
        , m_textColor(1,1,1,1)
        , m_backgroundColor(0.2f,0.2f,0.2f,1)
        , m_hoverBackgroundColor(0.3f,0.3f,0.3f,1)
        , m_pressedBackgroundColor(0.1f,0.1f,0.1f,1)
        , m_selectedBackgroundColor(0.3f,0.5f,0.9f,1)
        , m_borderColor(0,0,0,1)
        , m_borderWidth(0)
        , m_marginLeft(0), m_marginRight(0), m_marginTop(0), m_marginBottom(0)
        , m_paddingLeft(0), m_paddingRight(0), m_paddingTop(0), m_paddingBottom(0)
    {
    }

    UIStyle::UIStyle(const std::string& name)
        : UIStyle()
    {
        m_name = name;
    }

    void UIStyle::SetMargins(float left, float right, float top, float bottom)
    {
        m_marginLeft = left;
        m_marginRight = right;
        m_marginTop = top;
        m_marginBottom = bottom;
    }

    void UIStyle::SetPadding(float left, float right, float top, float bottom)
    {
        m_paddingLeft = left;
        m_paddingRight = right;
        m_paddingTop = top;
        m_paddingBottom = bottom;
    }

    UIStyle* UIStyle::GetDefaultStyle()
    {
        if (!s_defaultStyle) {
            s_defaultStyle = std::make_unique<UIStyle>("Default");
        }
        return s_defaultStyle.get();
    }

    void UIStyle::SetDefaultStyle(UIStyle* style)
    {
        s_defaultStyle.reset(style);
    }

} // namespace USE