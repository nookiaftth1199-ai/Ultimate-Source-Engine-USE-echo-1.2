// ============================================================
// Ultimate Source Engine - UI Screen Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIScreen.h"

namespace USE {

    UIScreen::UIScreen(const std::string& name)
        : UIElement()
        , m_name(name)
    {
    }

    UIScreen::~UIScreen()
    {
    }

    void UIScreen::OnShow()
    {
        // Default implementation does nothing
    }

    void UIScreen::OnHide()
    {
        // Default implementation does nothing
    }

    void UIScreen::OnUpdate(float deltaTime)
    {
        // Override in derived classes
    }

    void UIScreen::OnRender(IRenderDevice* device, Font* defaultFont)
    {
        // Override in derived classes
    }

    void UIScreen::Update(float deltaTime)
    {
        if (!m_visible) return;
        OnUpdate(deltaTime);
        UIElement::Update(deltaTime); // update children
    }

    void UIScreen::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;
        OnRender(device, defaultFont);
        UIElement::Render(device, defaultFont); // render children
    }

    bool UIScreen::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        return UIElement::OnMouseMove(x, y);
    }

    bool UIScreen::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        return UIElement::OnMouseButton(button, down, x, y);
    }

    bool UIScreen::OnKeyPress(int key, bool down)
    {
        if (!m_visible || !m_enabled) return false;
        return UIElement::OnKeyPress(key, down);
    }

    bool UIScreen::OnCharInput(unsigned int codepoint)
    {
        if (!m_visible || !m_enabled) return false;
        return UIElement::OnCharInput(codepoint);
    }

} // namespace USE