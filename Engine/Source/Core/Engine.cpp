// Engine.cpp – minimal core (no audio yet)
#include "stdafx.h"
#include "Engine.h"
#include "Core/Application.h"
#include "Core/Logger.h"

namespace USE
{
	Engine::Engine()
	{
		USE_LOG_INFO("Engine instance created.");
	}

	Engine::~Engine()
	{
		USE_LOG_INFO("Engine instance destroyed.");
	}

	// If your header declares GetAudio() etc., we simply return nullptr for now.
	// They are not called by the current Application.
}