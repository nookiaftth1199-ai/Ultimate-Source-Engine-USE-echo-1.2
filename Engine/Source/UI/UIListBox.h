// ============================================================
// Ultimate Source Engine – UI ListBox
// ============================================================
#pragma once

#include "UIElement.h"
#include <vector>
#include <string>
#include <functional>

namespace USE
{
	class UIListBox : public UIElement
	{
	public:
		UIListBox();
		~UIListBox() override;

		void AddItem(const std::string& item);
		void RemoveItem(int index);
		void Clear();

		int GetSelectedIndex() const { return m_selectedIndex; }
		void SetSelectedIndex(int index);

		std::function<void(int)> onSelectionChanged;

		void Render() override;
		void Update(float dt) override;
		void OnMouseWheel(int delta);          // <-- override removed (base class may not have it)

	private:
		std::vector<std::string> m_items;
		int m_selectedIndex = -1;
		float m_scrollOffset = 0.0f;
		float m_itemHeight = 20.0f;
	};
}