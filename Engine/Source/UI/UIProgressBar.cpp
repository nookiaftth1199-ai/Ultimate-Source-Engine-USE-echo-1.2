#include "stdafx.h"
#include "UIProgressBar.h"
#include "Math/Vector4.h"
#include <SDL_opengl.h>

namespace USE
{
	UIProgressBar::UIProgressBar() = default;
	UIProgressBar::~UIProgressBar() = default;

	void UIProgressBar::Render()
	{
		// stub
	}

	void UIProgressBar::SetProgress(float val)
	{
		if (val < 0) val = 0;
		if (val > 1) val = 1;
		m_progress = val;
	}

	float UIProgressBar::GetProgress() const { return m_progress; }
}