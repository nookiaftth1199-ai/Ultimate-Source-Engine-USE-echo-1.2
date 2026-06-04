#include "stdafx.h"
#include "UISystem.h"

namespace USE
{
	UISystem::UISystem() = default;
	UISystem::~UISystem() = default;

	bool UISystem::Initialize(RenderSystem*) { return true; }
	void UISystem::Shutdown() {}
	void UISystem::Update(float) {}
	void UISystem::Render() {}
}