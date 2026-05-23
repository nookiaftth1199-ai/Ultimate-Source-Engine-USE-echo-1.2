// ============================================================
// Ultimate Source Engine - Application
// ============================================================
//
// Base application class that manages the engine lifecycle,
// window, renderer, and main loop. Derive from this class
// to create your game.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Core/Window.h"
#include "Core/Timer.h"
#include "Core/ConfigManager.h"
#include "Core/FileSystem.h"
#include "Core/MemoryManager.h"
#include "Core/EventSystem.h"
#include "Core/ThreadPool.h"
#include "Core/JobSystem.h"
#include "Renderer/RenderSystem.h"
#include "Resources/ResourceManager.h"
#include "Input/InputManager.h"
#include "Audio/AudioSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Animation/AnimationSystem.h"
#include "AI/AISystem.h"
#include "UI/UISystem.h"
#include "Network/NetworkManager.h"
#include "Console/ConsoleSystem.h"
#include "Utility/Logger.h"
#include "Utility/Profiler.h"

namespace USE {

    // Forward declarations
    class GameMode;

    // Application configuration structure
    struct AppConfig {
        std::string title;
        int width;
        int height;
        bool fullscreen;
        bool vsync;
        std::string renderBackend;  // "opengl", "directx9", "vulkan", "auto"
        std::string assetPath;
        std::string logFile;
        int targetFPS;
        bool enableConsole;
        bool enableProfiler;

        AppConfig()
            : title("Ultimate Source Engine")
            , width(1280)
            , height(720)
            , fullscreen(false)
            , vsync(true)
            , renderBackend("auto")
            , assetPath("./Assets/")
            , logFile("engine.log")
            , targetFPS(0)
            , enableConsole(true)
            , enableProfiler(true)
        {}
    };

    // Main application class
    class Application {
    public:
        Application();
        virtual ~Application();

        // Initialize the engine with the given configuration
        bool Initialize(const AppConfig& config);
        void Shutdown();

        // Run the main loop
        void Run();

        // Request to quit
        void Quit();

        // Access to engine subsystems
        Window*           GetWindow() const        { return m_window; }
        RenderSystem*     GetRenderer() const      { return m_renderer; }
        ResourceManager*  GetResourceManager() const { return m_resourceManager; }
        InputManager*     GetInput() const         { return m_input; }
        AudioSystem*      GetAudio() const         { return m_audio; }
        PhysicsSystem*    GetPhysics() const       { return m_physics; }
        AnimationSystem*  GetAnimation() const     { return m_animation; }
        AISystem*         GetAI() const            { return m_ai; }
        UISystem*         GetUI() const            { return m_ui; }
        NetworkManager*   GetNetwork() const       { return m_network; }
        ConsoleSystem*    GetConsole() const       { return m_console; }
        EventSystem*      GetEvents() const        { return m_events; }
        ConfigManager*    GetConfig() const        { return m_config; }
        FileSystem*       GetFileSystem() const    { return m_fileSystem; }
        MemoryManager*    GetMemory() const        { return m_memory; }
        ThreadPool*       GetThreadPool() const    { return m_threadPool; }
        JobSystem*        GetJobSystem() const     { return m_jobSystem; }
        Profiler*         GetProfiler() const      { return m_profiler; }

        // Time management
        float GetDeltaTime() const     { return m_deltaTime; }
        float GetTotalTime() const      { return m_totalTime; }
        uint64_t GetFrameCount() const  { return m_frameCount; }
        float GetFPS() const             { return m_fps; }

        // Game mode
        void SetGameMode(GameMode* mode);
        GameMode* GetGameMode() const { return m_gameMode; }

    protected:
        // Override these in your derived game class
        virtual void OnInitialize() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}

        // Input callbacks (override as needed)
        virtual void OnKeyPressed(int key, int mods) {}
        virtual void OnKeyReleased(int key, int mods) {}
        virtual void OnMouseMove(int x, int y, int dx, int dy) {}
        virtual void OnMouseButton(int button, int action, int mods) {}
        virtual void OnMouseWheel(int delta) {}
        virtual void OnTextInput(const char* text) {}

        // Window callbacks
        virtual void OnWindowResized(int width, int height) {}
        virtual void OnWindowFocus(bool focused) {}
        virtual void OnWindowClosed() { Quit(); }

    private:
        // Engine subsystems
        Window*           m_window;
        RenderSystem*     m_renderer;
        ResourceManager*  m_resourceManager;
        InputManager*     m_input;
        AudioSystem*      m_audio;
        PhysicsSystem*    m_physics;
        AnimationSystem*  m_animation;
        AISystem*         m_ai;
        UISystem*         m_ui;
        NetworkManager*   m_network;
        ConsoleSystem*    m_console;
        EventSystem*      m_events;
        ConfigManager*    m_config;
        FileSystem*       m_fileSystem;
        MemoryManager*    m_memory;
        ThreadPool*       m_threadPool;
        JobSystem*        m_jobSystem;
        Profiler*         m_profiler;

        // Game mode
        GameMode*         m_gameMode;

        // Application state
        bool              m_running;
        bool              m_initialized;
        AppConfig         m_config;

        // Time
        Timer             m_timer;
        float             m_deltaTime;
        float             m_totalTime;
        uint64_t          m_frameCount;
        float             m_fps;
        float             m_fpsTimer;
        uint64_t          m_lastFrameTime;
        uint64_t          m_targetFrameTime; // microseconds per frame (0 = unlimited)

        // Private methods
        bool CreateSubsystems();
        void DestroySubsystems();
        void CalculateDeltaTime();
        void UpdateFPS();
        void LimitFrameRate();
        void ProcessEvents();

        // No copying
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
    };

    // Global application pointer (optional)
    extern Application* g_app;

} // namespace USE