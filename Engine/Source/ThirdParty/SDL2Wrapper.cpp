#include "stdafx.h"
#include "SDL2Wrapper.h"
#include "Core/Logger.h"
#include <SDL.h>

namespace USE
{
	bool SDL2Wrapper::Init()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		{
			USE_LOG_ERROR("SDL2Wrapper: SDL_Init failed: %s", SDL_GetError());
			return false;
		}
		return true;
	}

	void SDL2Wrapper::Shutdown()
	{
		SDL_Quit();
	}
}