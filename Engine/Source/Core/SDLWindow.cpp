#include "stdafx.h"
#include "SDLWindow.h"
#include "Core/Logger.h"

namespace USE
{
	SDLWindow::SDLWindow(const WindowDesc& desc)
		: m_title(desc.title), m_width(desc.width), m_height(desc.height), m_vsync(desc.vsync)
	{
	}

	SDLWindow::~SDLWindow()
	{
		Shutdown();
	}

	bool SDLWindow::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			USE_LOG_ERROR("SDLWindow: SDL_Init failed: %s", SDL_GetError());
			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		m_sdlWindow = SDL_CreateWindow(m_title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			m_width, m_height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		if (!m_sdlWindow)
		{
			USE_LOG_ERROR("SDLWindow: Failed to create window: %s", SDL_GetError());
			return false;
		}

		m_glContext = SDL_GL_CreateContext(m_sdlWindow);
		if (!m_glContext)
		{
			USE_LOG_ERROR("SDLWindow: Failed to create OpenGL context: %s", SDL_GetError());
			return false;
		}

		SDL_GL_MakeCurrent(m_sdlWindow, m_glContext);
		SDL_GL_SetSwapInterval(m_vsync ? 1 : 0);

		USE_LOG_INFO("SDLWindow initialized (%dx%d)", m_width, m_height);
		return true;
	}

	void SDLWindow::Shutdown()
	{
		if (m_glContext)
		{
			SDL_GL_DeleteContext(m_glContext);
			m_glContext = nullptr;
		}
		if (m_sdlWindow)
		{
			SDL_DestroyWindow(m_sdlWindow);
			m_sdlWindow = nullptr;
		}
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	void SDLWindow::SetTitle(const std::string& title)
	{
		m_title = title;
		SDL_SetWindowTitle(m_sdlWindow, title.c_str());
	}

	void SDLWindow::SetSize(int width, int height)
	{
		m_width = width;
		m_height = height;
		SDL_SetWindowSize(m_sdlWindow, width, height);
	}

	void SDLWindow::SetVSync(bool enabled)
	{
		m_vsync = enabled;
		SDL_GL_SetSwapInterval(enabled ? 1 : 0);
	}

	void SDLWindow::PumpEvents() {}

	void SDLWindow::SwapBuffers()
	{
		if (m_sdlWindow)
			SDL_GL_SwapWindow(m_sdlWindow);
	}
}