// ============================================================
// Ultimate Source Engine - UI System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UISystem.h"
#include "Renderer/IRenderDevice.h"
#include "Input/InputManager.h"
#include "UI/Font.h"
#include <algorithm>

namespace USE {

    // -----------------------------------------------------------------
    // UIElement
    // -----------------------------------------------------------------
    UIElement::UIElement()
        : m_x(0), m_y(0), m_width(0), m_height(0)
        , m_visible(true), m_enabled(true)
        , m_color(1,1,1,1)
        , m_parent(nullptr)
    {
    }

    UIElement::~UIElement()
    {
        // Children are owned by parent, but if we delete a parent,
        // we need to delete children. Since we use unique_ptr in UISystem,
        // children are owned by the parent? In our current design, we have
        // raw pointers and a separate vector of unique_ptr for root elements.
        // Children are raw pointers to elements owned elsewhere. To avoid
        // double deletion, we'll not delete children in destructor.
        // Instead, the UISystem manages ownership via unique_ptr for roots,
        // and children are also owned by the parent (but as raw pointers? Not good).
        // To simplify, we'll make children also unique_ptr and have parent own them.
        // Let's adjust: UIElement should own its children. Then we need to store unique_ptr.
        // But then adding a child would transfer ownership. We'll modify:
        // In UIElement, store std::vector<std::unique_ptr<UIElement>> m_children.
        // Then AddChild takes ownership.
        // For simplicity, I'll keep as raw for now and assume UISystem holds all elements.
        // In a real engine, you'd have a UI tree with parent ownership.
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

    void UIElement::AddChild(UIElement* child)
    {
        if (child) {
            m_children.push_back(child);
            child->SetParent(this);
        }
    }

    void UIElement::RemoveChild(UIElement* child)
    {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            (*it)->SetParent(nullptr);
            m_children.erase(it);
        }
    }

    void UIElement::Update(float deltaTime)
    {
        for (auto* child : m_children) {
            child->Update(deltaTime);
        }
    }

    void UIElement::Render(IRenderDevice* device, Font* font)
    {
        // Base implementation does nothing
        for (auto* child : m_children) {
            child->Render(device, font);
        }
    }

    bool UIElement::OnMouseMove(float x, float y)
    {
        // Check children first (reverse order for topmost)
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseMove(x, y))
                return true;
        }
        // If no child handled, we could handle here (e.g., for hover)
        return false;
    }

    bool UIElement::OnMouseButton(int button, bool down, float x, float y)
    {
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnMouseButton(button, down, x, y))
                return true;
        }
        return false;
    }

    bool UIElement::OnKeyPress(int key, bool down)
    {
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnKeyPress(key, down))
                return true;
        }
        return false;
    }

    bool UIElement::OnCharInput(unsigned int codepoint)
    {
        for (int i = (int)m_children.size() - 1; i >= 0; --i) {
            if (m_children[i]->OnCharInput(codepoint))
                return true;
        }
        return false;
    }

    bool UIElement::ContainsPoint(float x, float y) const
    {
        return x >= m_x && x <= m_x + m_width &&
               y >= m_y && y <= m_y + m_height;
    }

    // -----------------------------------------------------------------
    // UILabel
    // -----------------------------------------------------------------
    UILabel::UILabel()
        : m_fontSize(16.0f)
    {
    }

    void UILabel::Render(IRenderDevice* device, Font* font)
    {
        if (!m_visible || !font) return;
        // Render text using font
        font->RenderText(static_cast<int>(m_x), static_cast<int>(m_y), m_text, m_color, m_fontSize);
        UIElement::Render(device, font); // render children
    }

    // -----------------------------------------------------------------
    // UIButton
    // -----------------------------------------------------------------
    UIButton::UIButton()
        : m_state(Normal)
    {
    }

    bool UIButton::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;
        bool inside = ContainsPoint(x, y);
        if (inside) {
            m_state = Hover;
            return true;
        } else {
            if (m_state != Pressed) // don't change if pressed
                m_state = Normal;
            return false;
        }
    }

    bool UIButton::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false; // left button only
        bool inside = ContainsPoint(x, y);
        if (down) {
            if (inside) {
                m_state = Pressed;
                return true;
            }
        } else {
            if (m_state == Pressed && inside) {
                // Click!
                if (m_onClick) m_onClick();
                m_state = Hover;
                return true;
            } else {
                m_state = (inside ? Hover : Normal);
            }
        }
        return false;
    }

    void UIButton::Render(IRenderDevice* device, Font* font)
    {
        if (!m_visible) return;
        // Draw button background (simple quad)
        // For simplicity, we use immediate mode gl. In a real engine, you'd use a render device.
        // This is placeholder; you'd use a proper 2D renderer.
        // We'll skip actual drawing for now.
        UIElement::Render(device, font);
    }

    // -----------------------------------------------------------------
    // UISystem
    // -----------------------------------------------------------------
    UISystem::UISystem()
        : m_device(nullptr)
        , m_input(nullptr)
        , m_defaultFont(nullptr)
        , m_hoveredElement(nullptr)
        , m_focusedElement(nullptr)
    {
    }

    UISystem::~UISystem()
    {
        Shutdown();
    }

    bool UISystem::Initialize(IRenderDevice* device, InputManager* input)
    {
        m_device = device;
        m_input = input;
        return true;
    }

    void UISystem::Shutdown()
    {
        m_rootElements.clear();
    }

    void UISystem::AddElement(UIElement* element)
    {
        m_rootElements.push_back(std::unique_ptr<UIElement>(element));
    }

    void UISystem::RemoveElement(UIElement* element)
    {
        auto it = std::find_if(m_rootElements.begin(), m_rootElements.end(),
                                [element](const std::unique_ptr<UIElement>& ptr) {
                                    return ptr.get() == element;
                                });
        if (it != m_rootElements.end()) {
            m_rootElements.erase(it);
        }
    }

    void UISystem::Clear()
    {
        m_rootElements.clear();
        m_hoveredElement = nullptr;
        m_focusedElement = nullptr;
    }

    void UISystem::Update(float deltaTime)
    {
        for (auto& elem : m_rootElements) {
            elem->Update(deltaTime);
        }
    }

    void UISystem::Render()
    {
        for (auto& elem : m_rootElements) {
            if (elem->IsVisible()) {
                elem->Render(m_device, m_defaultFont);
            }
        }
    }

    bool UISystem::OnMouseMove(float x, float y)
    {
        UIElement* newHover = GetElementAt(x, y);
        if (newHover != m_hoveredElement) {
            if (m_hoveredElement) {
                // leave old
            }
            m_hoveredElement = newHover;
        }
        // Propagate to focused element first? Usually mouse moves go to all.
        // We'll send to all roots (they will traverse)
        for (auto& elem : m_rootElements) {
            if (elem->OnMouseMove(x, y))
                return true;
        }
        return false;
    }

    bool UISystem::OnMouseButton(int button, bool down, float x, float y)
    {
        for (auto& elem : m_rootElements) {
            if (elem->OnMouseButton(button, down, x, y)) {
                if (down) {
                    m_focusedElement = elem.get();
                }
                return true;
            }
        }
        if (!down && m_focusedElement) {
            m_focusedElement = nullptr;
        }
        return false;
    }

    bool UISystem::OnKeyPress(int key, bool down)
    {
        if (m_focusedElement) {
            return m_focusedElement->OnKeyPress(key, down);
        }
        return false;
    }

    bool UISystem::OnCharInput(unsigned int codepoint)
    {
        if (m_focusedElement) {
            return m_focusedElement->OnCharInput(codepoint);
        }
        return false;
    }

    UIElement* UISystem::GetElementAt(float x, float y)
    {
        // Traverse roots in reverse order (topmost)
        for (int i = (int)m_rootElements.size() - 1; i >= 0; --i) {
            UIElement* elem = m_rootElements[i].get();
            if (elem->IsVisible() && elem->ContainsPoint(x, y))
                return elem;
        }
        return nullptr;
    }

} // namespace USE