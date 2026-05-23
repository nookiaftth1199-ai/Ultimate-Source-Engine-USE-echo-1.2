// ============================================================
// Ultimate Source Engine - UI System
//============================================================
//
// Manages user interface elements, input handling, and rendering.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include <vector>
#include <memory>

namespace USE {

    // Forward declarations
    class IRenderDevice;
    class InputManager;
    class Font;

    // Base class for all UI elements
    class UIElement {
    public:
        UIElement();
        virtual ~UIElement();

        // Position and size (in screen coordinates)
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

        // Parent/child hierarchy
        void SetParent(UIElement* parent);
        UIElement* GetParent() const { return m_parent; }

        void AddChild(UIElement* child);
        void RemoveChild(UIElement* child);
        const std::vector<UIElement*>& GetChildren() const { return m_children; }

        // Update (called once per frame)
        virtual void Update(float deltaTime);

        // Render (called by UISystem)
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
        std::vector<UIElement*> m_children;
    };

    // -----------------------------------------------------------------
    // Label (text)
    // -----------------------------------------------------------------
    class UILabel : public UIElement {
    public:
        UILabel();
        void SetText(const std::string& text) { m_text = text; }
        const std::string& GetText() const { return m_text; }
        void SetFontSize(float size) { m_fontSize = size; }
        void Render(IRenderDevice* device, Font* font) override;

    private:
        std::string m_text;
        float       m_fontSize;
    };

    // -----------------------------------------------------------------
    // Button
    // -----------------------------------------------------------------
    class UIButton : public UIElement {
    public:
        UIButton();

        // Set callback when clicked
        void SetOnClick(std::function<void()> callback) { m_onClick = callback; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        void Render(IRenderDevice* device, Font* font) override;

    private:
        enum State { Normal, Hover, Pressed };
        State m_state;
        std::function<void()> m_onClick;
    };

    // -----------------------------------------------------------------
    // Main UI System
    // -----------------------------------------------------------------
    class UISystem {
    public:
        UISystem();
        ~UISystem();

        // Initialize with render device and input manager
        bool Initialize(IRenderDevice* device, InputManager* input);
        void Shutdown();

        // Add a root element (takes ownership)
        void AddElement(UIElement* element);

        // Remove an element (does not delete)
        void RemoveElement(UIElement* element);

        // Clear all elements
        void Clear();

        // Update all elements
        void Update(float deltaTime);

        // Render all visible elements
        void Render();

        // Input handling (forward from application)
        bool OnMouseMove(float x, float y);
        bool OnMouseButton(int button, bool down, float x, float y);
        bool OnKeyPress(int key, bool down);
        bool OnCharInput(unsigned int codepoint);

        // Get the element under mouse (for hover)
        UIElement* GetElementAt(float x, float y);

        // Set the default font
        void SetDefaultFont(Font* font) { m_defaultFont = font; }
        Font* GetDefaultFont() const { return m_defaultFont; }

    private:
        IRenderDevice* m_device;
        InputManager*  m_input;
        Font*          m_defaultFont;

        std::vector<std::unique_ptr<UIElement>> m_rootElements;
        UIElement* m_hoveredElement;
        UIElement* m_focusedElement;
    };

} // namespace USE