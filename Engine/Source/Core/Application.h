#pragma once

#include "Core/Timer.h"
#include <string>

namespace USE
{
	class Window;
	class RenderSystem;
	class ResourceManager;
	class InputManager;
	class FileSystem;

	struct AppConfig
	{
		std::string title = "Ultimate Source Engine";
		int width = 1280;
		int height = 720;
		bool fullscreen = false;
		bool vsync = true;
		int  targetFPS = 0;
		std::string renderBackend = "opengl";
		std::string assetPath = "Assets/";
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();

		bool Initialize(const AppConfig& config);
		void Shutdown();
		void Run();
		void Quit();

		Window*          GetWindow()          const { return m_window; }
		RenderSystem*    GetRenderer()        const { return m_renderer; }
		ResourceManager* GetResourceManager() const { return m_resourceManager; }
		InputManager*    GetInput()           const { return m_input; }
		FileSystem*      GetFileSystem()      const { return m_fileSystem; }

		static Application* GetInstance();

	protected:
		virtual void OnInitialize() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnShutdown() {}
		virtual void OnKeyPressed(int key, int mod) {}
		virtual void OnKeyReleased(int key, int mod) {}
		virtual void OnMouseMove(int x, int y, int dx, int dy) {}
		virtual void OnMouseButton(int button, int state, int clicks) {}
		virtual void OnMouseWheel(int delta) {}
		virtual void OnTextInput(const char* text) {}
		virtual void OnWindowResized(int width, int height) {}
		virtual void OnWindowFocus(bool focused) {}
		virtual void OnWindowClosed() {}

	private:
		bool CreateSubsystems();
		void DestroySubsystems();
		void CalculateDeltaTime();
		void UpdateFPS();
		void LimitFrameRate();
		void ProcessEvents();

		AppConfig m_config;
		Window*          m_window = nullptr;
		RenderSystem*    m_renderer = nullptr;
		ResourceManager* m_resourceManager = nullptr;
		InputManager*    m_input = nullptr;
		FileSystem*      m_fileSystem = nullptr;
		Timer m_timer;
		bool   m_running = false;
		bool   m_initialized = false;
		float  m_deltaTime = 0.0f;
		float  m_fps = 0.0f;
		float  m_fpsTimer = 0.0f;
		uint64_t m_lastFrameTime = 0;
		uint64_t m_targetFrameTime = 0;
		int    m_frameCount = 0;

		static Application* g_app;
	};
}