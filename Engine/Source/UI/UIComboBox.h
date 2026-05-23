// ============================================================
// Ultimate Source Engine - UI Combo Box
//============================================================
//
// A dropdown selection control. Displays a selected item and
// a button that opens a list of items to choose from.
// ============================================================

#pragma once

#include "stdafx.h"
#include "UIElement.h"
#include "UIListBox.h"
#include <functional>
#include <vector>
#include <memory>

namespace USE {

    class Font;

    class UIComboBox : public UIElement {
    public:
        UIComboBox();
        virtual ~UIComboBox() = default;

        // Item management
        void AddItem(const std::string& item);
        void InsertItem(size_t index, const std::string& item);
        void RemoveItem(size_t index);
        void ClearItems();
        size_t GetItemCount() const { return m_items.size(); }
        const std::string& GetItem(size_t index) const;

        // Selection
        void SetSelectedIndex(int index); // -1 for none
        int GetSelectedIndex() const { return m_selectedIndex; }
        std::string GetSelectedItem() const;

        // Dropdown control
        void OpenDropdown();
        void CloseDropdown();
        void ToggleDropdown();
        bool IsDropdownOpen() const { return m_dropdownOpen; }

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
        void SetButtonColor(const Color& color) { m_buttonColor = color; }
        void SetButtonHoverColor(const Color& color) { m_buttonHoverColor = color; }
        void SetListBoxBackgroundColor(const Color& color) { m_listBoxBgColor = color; }
        void SetListBoxTextColor(const Color& color) { m_listBoxTextColor = color; }
        void SetListBoxSelectedColor(const Color& color) { m_listBoxSelectedColor = color; }
        void SetListBoxHoverColor(const Color& color) { m_listBoxHoverColor = color; }

        // Overrides
        bool OnMouseMove(float x, float y) override;
        bool OnMouseButton(int button, bool down, float x, float y) override;
        void Update(float deltaTime) override;
        void Render(IRenderDevice* device, Font* defaultFont) override;

    private:
        std::vector<std::string> m_items;
        int m_selectedIndex;

        bool m_dropdownOpen;
        bool m_buttonHovered; // mouse over the dropdown button

        Font* m_font;

        // Colors
        Color m_bgColor;
        Color m_textColor;
        Color m_buttonColor;
        Color m_buttonHoverColor;

        // Colors for the internal listbox (we'll create one on the fly)
        Color m_listBoxBgColor;
        Color m_listBoxTextColor;
        Color m_listBoxSelectedColor;
        Color m_listBoxHoverColor;

        std::function<void(int, const std::string&)> m_onSelectionChanged;

        // Helper: get the rectangle of the dropdown button
        void GetButtonRect(float& left, float& top, float& right, float& bottom) const;

        // Helper: create a listbox for dropdown (called when opening)
        void CreateDropdownListBox();

        // The dropdown listbox (as a child element)
        std::unique_ptr<UIListBox> m_dropdownListBox;
    };

} // namespace USE