#include <SDL.h>
#include <iostream>

bool TestSDL() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Init FAILED: " << SDL_GetError() << std::endl;
        return false;
    }
    
    std::cout << "SDL Initialized Successfully!" << std::endl;
    std::cout << "SDL Version: " << SDL_MAJOR_VERSION << "." 
              << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL << std::endl;
    
    SDL_Window* window = SDL_CreateWindow("SDL Test", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    
    if(!window) {
        std::cout << "Window creation FAILED: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    std::cout << "SDL Window created successfully!" << std::endl;
    
    SDL_Delay(2000);  // Show window for 2 seconds
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return true;
}