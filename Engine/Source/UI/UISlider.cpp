#include "stdafx.h"
#include "UISlider.h"
#include <SDL_opengl.h>

namespace USE
{
	UISlider::UISlider() = default;
	UISlider::~UISlider() = default;

	void UISlider::Render()
	{
		// stub – no GL drawing
	}

	void UISlider::SetValue(float val)
	{
		if (val < m_min) val = m_min;
		if (val > m_max) val = m_max;
		m_value = val;
	}

	float UISlider::GetValue() const { return m_value; }
}