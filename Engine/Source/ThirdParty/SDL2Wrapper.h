// ============================================================
// SDL2Wrapper.h
// ============================================================
#pragma once
#include <SDL.h>

namespace USE {
    class SDL2Wrapper {
    public:
        static bool Initialize();
        static void Shutdown();
        static SDL_Window* CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags);
        static void DestroyWindow(SDL_Window* window);
        static SDL_GLContext CreateGLContext(SDL_Window* window);
        static void DeleteGLContext(SDL_GLContext context);
        static void SwapWindow(SDL_Window* window);
    };
}