// ============================================================
// Ultimate Source Engine - UI Element
// ============================================================
//
// Base class for all user interface elements.
// Provides position, size, visibility, and parent-child hierarchy.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include <vector>
#include <functional>
#include <memory>

namespace USE {

    // Forward declarations
    class Font;
    class IRenderDevice;

    class UIElement {
    public:
        UIElement();
        virtual ~UIElement();

        // Position and size
        void SetPosition(float x, float y);
        void SetSize(float width, float height);
        virtual void SetBounds(float x, float y, float width, float height);
        float GetX() const { return m_x; }
        float GetY() const { return m_y; }
        float GetWidth() const { return m_width; }
        float GetHeight() const { return m_height; }

        // Visibility
        void SetVisible(bool visible) { m_visible = visible; }
        bool IsVisible() const { return m_visible; }

        // Enable/disable interaction
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        // Color
        void SetColor(const Color& color) { m_color = color; }
        const Color& GetColor() const { return m_color; }

        // Parent/child hierarchy (parent owns children)
        void SetParent(UIElement* parent);
        UIElement* GetParent() const { return m_parent; }
        void AddChild(std::unique_ptr<UIElement> child);
        UIElement* AddChild(UIElement* child); // convenience, transfers ownership
        void RemoveChild(UIElement* child);
        const std::vector<std::unique_ptr<UIElement>>& GetChildren() const { return m_children; }

        // Update and render
        virtual void Update(float deltaTime);
        virtual void Render(IRenderDevice* device, Font* font);

        // Input handling (coordinates in screen space)
        virtual bool OnMouseMove(float x, float y);
        virtual bool OnMouseButton(int button, bool down, float x, float y);
        virtual bool OnKeyPress(int key, bool down);
        virtual bool OnCharInput(unsigned int codepoint);

        // Hit test
        virtual bool ContainsPoint(float x, float y) const;

    protected:
        float m_x, m_y;
        float m_width, m_height;
        bool  m_visible;
        bool  m_enabled;
        Color m_color;

        UIElement* m_parent;
        std::vector<std::unique_ptr<UIElement>> m_children;
    };

} // namespace USE