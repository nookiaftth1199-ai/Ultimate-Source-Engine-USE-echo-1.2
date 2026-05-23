// ============================================================
// SDL2Wrapper.cpp
// ============================================================
#include "SDL2Wrapper.h"
#include "Core/Logger.h"

namespace USE {
    bool SDL2Wrapper::Initialize() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
            USE_LOG_ERROR("SDL2 failed to initialize: %s", SDL_GetError());
            return false;
        }
        USE_LOG_INFO("SDL2 initialized");
        return true;
    }

    void SDL2Wrapper::Shutdown() {
        SDL_Quit();
        USE_LOG_INFO("SDL2 shut down");
    }

    SDL_Window* SDL2Wrapper::CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags) {
        SDL_Window* win = SDL_CreateWindow(title, x, y, w, h, flags);
        if (!win) USE_LOG_ERROR("SDL_CreateWindow failed: %s", SDL_GetError());
        return win;
    }

    void SDL2Wrapper::DestroyWindow(SDL_Window* window) {
        if (window) SDL_DestroyWindow(window);
    }

    SDL_GLContext SDL2Wrapper::CreateGLContext(SDL_Window* window) {
        SDL_GLContext ctx = SDL_GL_CreateContext(window);
        if (!ctx) USE_LOG_ERROR("SDL_GL_CreateContext failed: %s", SDL_GetError());
        return ctx;
    }

    void SDL2Wrapper::DeleteGLContext(SDL_GLContext context) {
        if (context) SDL_GL_DeleteContext(context);
    }

    void SDL2Wrapper::SwapWindow(SDL_Window* window) {
        if (window) SDL_GL_SwapWindow(window);
    }
}