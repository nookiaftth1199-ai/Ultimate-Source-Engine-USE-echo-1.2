#include "stdafx.h"
#include "UITextBox.h"

namespace USE
{
	UITextBox::UITextBox() = default;
	UITextBox::~UITextBox() = default;

	void UITextBox::Render()
	{
		// stub
	}

	void UITextBox::SetText(const std::string& text)
	{
		m_text = text;
	}

	const std::string& UITextBox::GetText() const { return m_text; }
}