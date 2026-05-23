// ============================================================
// Ultimate Source Engine - UI List Box
//============================================================
//
// A scrollable list of text items. Supports single selection,
// mouse click selection, and scroll wheel navigation.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include <vector>
#include <functional>

namespace USE {

    class Font;

    class UIListBox : public UIElement {
    public:
        UIListBox();
        virtual ~UIListBox() = default;

        // Item management
        void AddItem(const std::string& item);
        void InsertItem(size_t index, const std::string& item);
        void RemoveItem(size_t index);
        void ClearItems();

        size_t GetItemCount() const { return m_items.size(); }
        const std::string& GetItem(size_t index) const;

        // Selection (single selection)
        void SetSelectedIndex(int index); // -1 for none
        int GetSelectedIndex() const { return m_selectedIndex; }
        std::string GetSelectedItem() const;

        // Callback when selection changes
        void SetOnSelectionChanged(std::function<void(int index, const std::string& item)> callback) {
            m_onSelectionChanged = callback;
        }

        // Font
        void SetFont(Font* font) { m_font = font; }
        Font* GetFont() const { return m_font; }

        // Colors
        void SetBackgroundColor(const Color& color) { m_bgColor = color; }
        void SetTextColor(const Color& color) { m_textColor = color; }
        void SetSelectedColor(const Color& color) { m_selectedColor = color; }
        void SetHoverColor(const Color& color) { m_hoverColor = color; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        bool OnMouseWheel(float delta) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        struct Item {
            std::string text;
        };
        std::vector<Item> m_items;

        int  m_selectedIndex;
        int  m_hoverIndex;          // index of item under mouse (-1 if none)
        int  m_scrollOffset;         // how many items have been scrolled past

        Font* m_font;

        Color m_bgColor;
        Color m_textColor;
        Color m_selectedColor;
        Color m_hoverColor;

        std::function<void(int, const std::string&)> m_onSelectionChanged;

        // Helper to compute visible range and item rectangles
        bool GetItemRect(size_t index, float& x, float& y, float& w, float& h) const;
        int  GetItemIndexAt(float localY) const; // returns -1 if none
    };

} // namespace USE