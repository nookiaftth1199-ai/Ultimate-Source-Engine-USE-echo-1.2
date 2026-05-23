// ============================================================
// Ultimate Source Engine - UI Menu
//============================================================
//
// A container that manages a list of selectable items (e.g., buttons).
// Provides keyboard/gamepad navigation (up/down) and activation.
// Optionally renders a background and title.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <vector>
#include <functional>

namespace USE {

    class Font;

    class UIMenu : public UIElement {
    public:
        UIMenu();
        virtual ~UIMenu() = default;

        // Menu title (optional)
        void SetTitle(const std::string& title) { m_title = title; }
        const std::string& GetTitle() const { return m_title; }

        // Add a UI element as a menu item. The element is added as a child.
        // If the element is selectable (by default, any element that can handle mouse clicks),
        // it will be included in keyboard navigation.
        void AddItem(UIElement* item, bool selectable = true);

        // Clear all items (children are destroyed)
        void ClearItems();

        // Navigation and selection
        void SetSelectedIndex(int index);
        int GetSelectedIndex() const { return m_selectedIndex; }

        // Callback when an item is activated (e.g., clicked or Enter pressed)
        void SetOnActivated(std::function<void(int index, UIElement* item)> callback) {
            m_onActivated = callback;
        }

        // Colors
        void SetBackgroundColor(const Color& color) { m_bgColor = color; }
        void SetTitleColor(const Color& color) { m_titleColor = color; }
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Overrides
        bool OnKeyPress(int key, bool down) override;
        void Update(float deltaTime) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    protected:
        // Recalculate layout (e.g., position items vertically)
        virtual void UpdateLayout();

    private:
        std::string m_title;
        Font*       m_font;

        Color m_bgColor;
        Color m_titleColor;

        // List of child indices that are selectable (in order)
        std::vector<size_t> m_selectableIndices;
        int                 m_selectedIndex; // index into m_selectableIndices

        std::function<void(int, UIElement*)> m_onActivated;

        // Helper to get currently selected item (if any)
        UIElement* GetSelectedItem() const;
    };

} // namespace USE