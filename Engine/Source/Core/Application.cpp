#include "stdafx.h"
#include "Application.h"
#include "Core/SDLWindow.h"
#include "Core/FileSystem.h"
#include "Renderer/RenderSystem.h"
#include "Input/InputManager.h"
#include "Renderer/OpenGL/GLDevice.h"     // <-- NEW
#include <SDL.h>
#include <SDL_opengl.h>

namespace USE {

	Application* Application::g_app = nullptr;

	Application::Application() { g_app = this; }
	Application::~Application() { Shutdown(); }

	bool Application::Initialize(const AppConfig& config) {
		if (m_initialized) return true;
		m_config = config;
		if (!CreateSubsystems()) return false;
		m_running = true;
		m_initialized = true;
		OnInitialize();
		return true;
	}

	void Application::Shutdown() {
		if (!m_initialized) return;
		OnShutdown();
		DestroySubsystems();
		m_initialized = false;
		m_running = false;
	}

	void Application::Run() {
		if (!m_initialized) return;
		m_timer.Reset();
		m_lastFrameTime = m_timer.GetElapsedTicks();

		// ----- Create and initialise the real OpenGL device -----
		GLDevice glDevice;
		glDevice.Initialize(m_window->GetNativeHandle(),
			m_config.width, m_config.height,
			m_config.vsync);

		while (m_running) {
			CalculateDeltaTime();
			ProcessEvents();
			if (m_input) m_input->Update();
			OnUpdate(m_deltaTime);

			// --- Use GLDevice for clearing & presenting ---
			glDevice.BeginFrame();
			glDevice.Clear(true, true, false,
				Vector4(0.1f, 0.1f, 0.3f, 1.0f),  // dark blue
				1.0f, 0);

			// --- Draw the triangle (still immediate‑mode) ---
			// This part will be replaced with shaders in the next step.
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, m_config.width, m_config.height, 0, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(400, 200);
			glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(600, 500);
			glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(200, 500);
			glEnd();

			OnRender();
			glDevice.Present();   // GLDevice swaps the buffers

			UpdateFPS();
			LimitFrameRate();
			++m_frameCount;
		}
	}

	void Application::Quit() { m_running = false; }
	Application* Application::GetInstance() { return g_app; }

	bool Application::CreateSubsystems() {
		m_fileSystem = new FileSystem();
		m_fileSystem->Initialize(m_config.assetPath);

		WindowDesc wdesc;
		wdesc.title = m_config.title;
		wdesc.width = m_config.width;
		wdesc.height = m_config.height;
		wdesc.vsync = m_config.vsync;
		m_window = new SDLWindow(wdesc);
		if (!m_window->Initialize()) return false;

		m_renderer = new RenderSystem();
		m_renderer->Initialize(RenderBackend::OpenGL, m_window, m_config.vsync);

		m_input = new InputManager();
		m_input->Initialize();

		return true;
	}

	void Application::DestroySubsystems() {
		delete m_input;
		delete m_renderer;
		delete m_window;
		delete m_fileSystem;
	}

	void Application::CalculateDeltaTime() {
		uint64_t now = m_timer.GetElapsedTicks();
		m_deltaTime = (now - m_lastFrameTime) / 1000000.0f;
		if (m_deltaTime > 0.25f) m_deltaTime = 0.25f;
		m_lastFrameTime = now;
	}

	void Application::UpdateFPS() {
		m_fpsTimer += m_deltaTime;
		if (m_fpsTimer >= 1.0f) { m_fps = 1.0f / m_deltaTime; m_fpsTimer = 0.0f; }
	}

	void Application::LimitFrameRate() {
		if (m_targetFrameTime == 0) return;
		uint64_t now = m_timer.GetElapsedTicks();
		uint64_t elapsed = now - m_lastFrameTime;
		if (elapsed < m_targetFrameTime) Timer::SleepMicroseconds(m_targetFrameTime - elapsed);
	}

	void Application::ProcessEvents() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT: Quit(); break;
			case SDL_KEYDOWN: OnKeyPressed(e.key.keysym.sym, e.key.keysym.mod); break;
			case SDL_KEYUP: OnKeyReleased(e.key.keysym.sym, e.key.keysym.mod); break;
			case SDL_MOUSEMOTION: OnMouseMove(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel); break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP: OnMouseButton(e.button.button, e.button.state, e.button.clicks); break;
			case SDL_MOUSEWHEEL: OnMouseWheel(e.wheel.y); break;
			case SDL_TEXTINPUT: OnTextInput(e.text.text); break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) OnWindowResized(e.window.data1, e.window.data2);
				else if (e.window.event == SDL_WINDOWEVENT_CLOSE) OnWindowClosed();
				break;
			}
		}
	}

} // namespace USE