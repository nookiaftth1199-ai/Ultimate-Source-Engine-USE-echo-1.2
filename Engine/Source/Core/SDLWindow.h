#pragma once
#include "Window.h"
#include <SDL.h>
#include <string>

namespace USE
{
	class SDLWindow : public Window
	{
	public:
		SDLWindow(const WindowDesc& desc);
		~SDLWindow() override;

		bool Initialize() override;
		void Shutdown() override;

		void SetTitle(const std::string& title) override;
		void SetSize(int width, int height) override;
		void SetVSync(bool enabled) override;

		void PumpEvents() override;
		void SwapBuffers() override;

		int  GetWidth()  const override { return m_width; }
		int  GetHeight() const override { return m_height; }
		void* GetNativeHandle() const override { return m_sdlWindow; }
		bool IsValid() const override { return m_sdlWindow != nullptr; }

		SDL_Window* GetSDLWindow() const { return m_sdlWindow; }

	private:
		SDL_Window*   m_sdlWindow = nullptr;
		SDL_GLContext m_glContext = nullptr;
		std::string   m_title;
		int           m_width = 1280;
		int           m_height = 720;
		bool          m_vsync = true;
	};
}