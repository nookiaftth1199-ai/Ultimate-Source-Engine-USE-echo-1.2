// ============================================================
// ImGuiIntegration.h
// ============================================================
#pragma once
#include <SDL.h>

namespace USE {
    class ImGuiIntegration {
    public:
        static ImGuiIntegration& Get();

        bool Initialize(SDL_Window* window);
        void Shutdown();
        void BeginFrame();
        void EndFrame();
        void ProcessEvent(const SDL_Event& event);

        bool WantCaptureMouse() const;
        bool WantCaptureKeyboard() const;
    };
}