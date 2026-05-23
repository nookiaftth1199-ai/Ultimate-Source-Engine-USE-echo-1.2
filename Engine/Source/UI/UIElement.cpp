// ============================================================
// Ultimate Source Engine - UI Element Implementation
// ============================================================

#include "stdafx.h"
#include "UIElement.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include <algorithm>

namespace USE {

    UIElement::UIElement()
        : m_x(0), m_y(0), m_width(0), m_height(0)
        , m_visible(true), m_enabled(true)
        , m_color(1,1,1,1)
        , m_parent(nullptr)
    {
    }

    UIElement::~UIElement()
    {
    }

    void UIElement::SetPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void UIElement::SetSize(float width, float height)
    {
        m_width = width;
        m_height = height;
    }

    void UIElement::SetBounds(float x, float y, float width, float height)
    {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }

    void UIElement::SetParent(UIElement* parent)
    {
        m_parent = parent;
    }

    void UIElement::AddChild(std::unique_ptr<UIElement> child)
    {
        child->SetParent(this);
        m_children.push_back(std::move(child));
    }

    UIElement* UIElement::AddChild(UIElement* child)
    {
        std::unique_ptr<UIElement> ptr(child);
        AddChild(std::move(ptr));
        return child;
    }

    void UIElement::RemoveChild(UIElement* child)
    {
        auto it = std::find_if(m_children.begin(), m_children.end(),
                                [child](const std::unique_ptr<UIElement>& ptr) {
                                    return ptr.get() == child;
                                });
        if (it != m_children.end()) {
            (*it)->SetParent(nullptr);
            m_children.erase(it);
        }
    }

    void UIElement::Update(float deltaTime)
    {
        for (auto& child : m_children) {
            child->Update(deltaTime);
        }
    }

    void UIElement::Render(IRenderDevice* device, Font* font)
    {
        for (auto& child : m_children) {
            child->Render(device, font);
        }
    }

    bool UIElement::OnMouseMove(float x, float y)
    {
        // Traverse children in reverse order (topmost first)
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseMove(x, y)) {
                return true;
            }
        }
        return false;
    }

    bool UIElement::OnMouseButton(int button, bool down, float x, float y)
    {
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseButton(button, down, x, y)) {
                return true;
            }
        }
        return false;
    }

    bool UIElement::OnKeyPress(int key, bool down)
    {
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; --i) {
            if (m_children[i]->OnKeyPress(key, down)) {
                return true;
            }
        }
        return false;
    }

    bool UIElement::OnCharInput(unsigned int codepoint)
    {
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; --i) {
            if (m_children[i]->OnCharInput(codepoint)) {
                return true;
            }
        }
        return false;
    }

    bool UIElement::ContainsPoint(float x, float y) const
    {
        return x >= m_x && x <= m_x + m_width &&
               y >= m_y && y <= m_y + m_height;
    }

} // namespace USE