#pragma once

#include "Core/Application.h"

namespace USE
{
	// Minimal Engine class – no audio/physics/... getters yet.
	// They will be added back when those subsystems are enabled.
	class Engine
	{
	public:
		Engine();
		~Engine();

		Application* GetApplication() const { return Application::GetInstance(); }
	};
}