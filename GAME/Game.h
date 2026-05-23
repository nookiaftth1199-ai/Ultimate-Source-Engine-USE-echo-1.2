#include "Game.h"
#include "USE_Engine/Core/Application.h"
#include "USE_Engine/Renderer/OpenGL/GLDevice.h"

using namespace USE;  // Changed from SRC_Engine

class UltimateGame : public Application {  // Renamed from MyGame
public:
    UltimateGame(const char* title, int width, int height) 
        : Application(title, width, height) {
    }
    
    void OnInitialize() override {
        // Ultimate Source Engine initialization
        printf("========================================\n");
        printf("  Ultimate Source Engine - Game Mode\n");
        printf("========================================\n");
        printf("Engine: %s\n", GetEngineName());
        printf("Version: %s\n", GetEngineVersion());
        printf("Renderer: OpenGL 2.0\n");
        printf("Platform: Windows 32-bit\n");
        printf("========================================\n");
        
        if(g_pUSE_Console) {
            g_pUSE_Console->Print("Game initialized successfully!");
            g_pUSE_Console->PrintWarning("Ultimate Source Engine ready");
        }
    }
    
    void OnUpdate(float deltaTime) override {
        // Game logic update
    }
    
    void OnRender() override {
        // Ultimate Source Engine - Render with style!
        glClearColor(0.0f, 0.2f, 0.4f, 1.0f); // Signature blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void OnShutdown() override {
        if(g_pUSE_Console) {
            g_pUSE_Console->Print("Ultimate Source Engine shutting down...");
        }
        printf("Game shutdown complete!\n");
    }
};

// Factory function
Game* CreateGame() {
    return new UltimateGame("Ultimate Source Engine Game", 1280, 720);
}