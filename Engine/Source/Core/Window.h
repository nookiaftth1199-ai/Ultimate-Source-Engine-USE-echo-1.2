#pragma once
#include <string>
#include <cstdint>

namespace USE
{
	struct WindowDesc
	{
		std::string title = "USE Engine";
		int width = 1280;
		int height = 720;
		bool fullscreen = false;
		bool vsync = true;
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void PumpEvents() = 0;
		virtual void SwapBuffers() = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetSize(int width, int height) = 0;
		virtual void SetVSync(bool enabled) = 0;

		virtual int  GetWidth()  const = 0;
		virtual int  GetHeight() const = 0;
		virtual void* GetNativeHandle() const = 0;
		virtual bool IsValid() const = 0;
	};
}