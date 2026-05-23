// ============================================================
// Ultimate Source Engine - UI Combo Box Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "UIComboBox.h"
#include "Renderer/IRenderDevice.h"
#include "UI/Font.h"
#include "Math/MathUtils.h"

namespace USE {

    UIComboBox::UIComboBox()
        : m_selectedIndex(-1)
        , m_dropdownOpen(false)
        , m_buttonHovered(false)
        , m_font(nullptr)
        , m_bgColor(0.2f, 0.2f, 0.2f, 1.0f)
        , m_textColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_buttonColor(0.3f, 0.3f, 0.3f, 1.0f)
        , m_buttonHoverColor(0.5f, 0.5f, 0.5f, 1.0f)
        , m_listBoxBgColor(0.25f, 0.25f, 0.25f, 1.0f)
        , m_listBoxTextColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_listBoxSelectedColor(0.3f, 0.5f, 0.9f, 1.0f)
        , m_listBoxHoverColor(0.4f, 0.4f, 0.4f, 1.0f)
    {
    }

    void UIComboBox::AddItem(const std::string& item)
    {
        m_items.push_back(item);
    }

    void UIComboBox::InsertItem(size_t index, const std::string& item)
    {
        if (index <= m_items.size()) {
            m_items.insert(m_items.begin() + index, item);
        }
    }

    void UIComboBox::RemoveItem(size_t index)
    {
        if (index < m_items.size()) {
            m_items.erase(m_items.begin() + index);
            if (m_selectedIndex == (int)index) {
                m_selectedIndex = -1;
            } else if (m_selectedIndex > (int)index) {
                m_selectedIndex--;
            }
        }
    }

    void UIComboBox::ClearItems()
    {
        m_items.clear();
        m_selectedIndex = -1;
    }

    const std::string& UIComboBox::GetItem(size_t index) const
    {
        static std::string empty;
        if (index < m_items.size())
            return m_items[index];
        return empty;
    }

    void UIComboBox::SetSelectedIndex(int index)
    {
        if (index < -1 || index >= (int)m_items.size())
            return;
        if (m_selectedIndex != index) {
            m_selectedIndex = index;
            if (m_onSelectionChanged) {
                std::string item = (index >= 0) ? m_items[index] : "";
                m_onSelectionChanged(index, item);
            }
        }
    }

    std::string UIComboBox::GetSelectedItem() const
    {
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size())
            return m_items[m_selectedIndex];
        return "";
    }

    void UIComboBox::OpenDropdown()
    {
        if (m_dropdownOpen || m_items.empty()) return;
        m_dropdownOpen = true;
        CreateDropdownListBox();
    }

    void UIComboBox::CloseDropdown()
    {
        if (!m_dropdownOpen) return;
        m_dropdownOpen = false;
        m_dropdownListBox.reset(); // destroy the listbox
    }

    void UIComboBox::ToggleDropdown()
    {
        if (m_dropdownOpen)
            CloseDropdown();
        else
            OpenDropdown();
    }

    void UIComboBox::GetButtonRect(float& left, float& top, float& right, float& bottom) const
    {
        // Button is a square at the right side, say 20 pixels wide
        float buttonSize = m_height; // or maybe a fixed size
        left = m_x + m_width - buttonSize;
        top = m_y;
        right = m_x + m_width;
        bottom = m_y + buttonSize;
    }

    void UIComboBox::CreateDropdownListBox()
    {
        if (m_items.empty()) return;

        // Create a listbox as a child
        auto listBox = std::make_unique<UIListBox>();
        listBox->SetFont(m_font);
        listBox->SetBackgroundColor(m_listBoxBgColor);
        listBox->SetTextColor(m_listBoxTextColor);
        listBox->SetSelectedColor(m_listBoxSelectedColor);
        listBox->SetHoverColor(m_listBoxHoverColor);

        for (const auto& item : m_items) {
            listBox->AddItem(item);
        }
        listBox->SetSelectedIndex(m_selectedIndex);

        // Set its bounds (below the combo box, same width, height based on number of items)
        float listHeight = listBox->GetItemHeight() * (float)m_items.size();
        // Clamp to max height (e.g., 200)
        if (listHeight > 200) listHeight = 200;
        listBox->SetBounds(m_x, m_y + m_height, m_width, listHeight);

        // Set callback to select item when clicked
        listBox->SetOnSelectionChanged([this](int index, const std::string& item) {
            SetSelectedIndex(index);
            CloseDropdown();
        });

        m_dropdownListBox = std::move(listBox);
        // The listbox will be rendered as a child (UIElement::Render handles children)
        // We need to add it as a child so it gets mouse events automatically.
        // Since we manage m_dropdownListBox separately, we could also forward events manually.
        // For simplicity, we'll just forward mouse events manually in OnMouseButton.
    }

    bool UIComboBox::OnMouseMove(float x, float y)
    {
        if (!m_visible || !m_enabled) return false;

        float l, t, r, b;
        GetButtonRect(l, t, r, b);
        m_buttonHovered = (x >= l && x <= r && y >= t && y <= b);

        // If dropdown is open, forward to listbox
        if (m_dropdownOpen && m_dropdownListBox) {
            // Convert to listbox local coordinates
            float localX = x - m_dropdownListBox->GetX();
            float localY = y - m_dropdownListBox->GetY();
            if (m_dropdownListBox->ContainsPoint(x, y)) {
                m_dropdownListBox->OnMouseMove(x, y);
                return true;
            }
        }
        return m_buttonHovered;
    }

    bool UIComboBox::OnMouseButton(int button, bool down, float x, float y)
    {
        if (!m_visible || !m_enabled || button != 0) return false;

        if (down) {
            // Check if click on button
            float l, t, r, b;
            GetButtonRect(l, t, r, b);
            if (x >= l && x <= r && y >= t && y <= b) {
                ToggleDropdown();
                return true;
            }

            // If dropdown is open, forward to listbox
            if (m_dropdownOpen && m_dropdownListBox) {
                if (m_dropdownListBox->ContainsPoint(x, y)) {
                    return m_dropdownListBox->OnMouseButton(button, down, x, y);
                } else {
                    // Click outside closes dropdown
                    CloseDropdown();
                }
            }
        }
        return false;
    }

    void UIComboBox::Update(float deltaTime)
    {
        // Update children (like dropdown listbox) if needed
        if (m_dropdownOpen && m_dropdownListBox) {
            m_dropdownListBox->Update(deltaTime);
        }
    }

    void UIComboBox::Render(IRenderDevice* device, Font* defaultFont)
    {
        if (!m_visible) return;

        Font* fontToUse = m_font ? m_font : defaultFont;

        // Draw background
        glBegin(GL_QUADS);
        glColor4f(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        glVertex2f(m_x, m_y);
        glVertex2f(m_x + m_width, m_y);
        glVertex2f(m_x + m_width, m_y + m_height);
        glVertex2f(m_x, m_y + m_height);
        glEnd();

        // Draw selected text
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size() && fontToUse) {
            std::string text = m_items[m_selectedIndex];
            float textX = m_x + 5;
            float textY = m_y + (m_height - fontToUse->GetLineHeight()) * 0.5f;
            fontToUse->RenderText(static_cast<int>(textX), static_cast<int>(textY), text, m_textColor);
        }

        // Draw dropdown button
        float l, t, r, b;
        GetButtonRect(l, t, r, b);
        Color buttonColor = m_buttonHovered ? m_buttonHoverColor : m_buttonColor;
        glBegin(GL_QUADS);
        glColor4f(buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        glVertex2f(l, t);
        glVertex2f(r, t);
        glVertex2f(r, b);
        glVertex2f(l, b);
        glEnd();

        // Draw arrow (simple triangle)
        float arrowSize = 5.0f;
        float arrowCenterX = (l + r) * 0.5f;
        float arrowCenterY = (t + b) * 0.5f;
        glBegin(GL_TRIANGLES);
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glVertex2f(arrowCenterX - arrowSize, arrowCenterY - arrowSize * 0.5f);
        glVertex2f(arrowCenterX + arrowSize, arrowCenterY - arrowSize * 0.5f);
        glVertex2f(arrowCenterX, arrowCenterY + arrowSize * 0.5f);
        glEnd();

        // Render dropdown listbox if open
        if (m_dropdownOpen && m_dropdownListBox) {
            m_dropdownListBox->Render(device, defaultFont);
        }

        // Render children (if any)
        UIElement::Render(device, defaultFont);
    }

} // namespace USE