// ============================================================
// Ultimate Source Engine - Engine Core
// Version: 1.2 "Echo"
// ============================================================
#pragma once

#include "stdafx.h"
#include "Core/Singleton.h"
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
#include "Entity/World.h"
#include "Scene/Scene.h"
#include "Animation/AnimationSystem.h"
#include "AI/AISystem.h"
#include "UI/UISystem.h"
#include "Network/NetworkManager.h"
#include "Console/ConsoleSystem.h"
#include "Utility/Logger.h"
#include "Utility/Profiler.h"
#include "Video/VideoDecoder.h"          // 🆕 video subsystem
#include "Resources/MapLoader.h"         // 🆕 map loader

namespace USE {

    // Forward declarations
    class GameMode;
    class Window;

    // Engine configuration structure (extended for 1.2)
    struct EngineConfig {
        std::string windowTitle;
        int windowWidth;
        int windowHeight;
        bool fullscreen;
        bool vsync;
        int fpsLimit;
        std::string renderBackend;   // "opengl", "directx9", "vulkan", "auto"
        std::string assetPath;
        std::string logFile;
        int threadCount;
        bool enableProfiler;
        bool enableConsole;
        bool enableDebugDraw;

        // 1.2 new options
        bool enableVideoPlayback;
        bool enableMapFormat;
        bool enableForgeAssetManager;

        EngineConfig()
            : windowTitle("Ultimate Source Engine")
            , windowWidth(1280)
            , windowHeight(720)
            , fullscreen(false)
            , vsync(true)
            , fpsLimit(0)
            , renderBackend("auto")
            , assetPath("./ASSETS/")
            , logFile("engine.log")
            , threadCount(0)           // 0 = auto-detect
            , enableProfiler(true)
            , enableConsole(true)
            , enableDebugDraw(true)
            , enableVideoPlayback(true)
            , enableMapFormat(true)
            , enableForgeAssetManager(true)
        {}
    };

    class Engine : public Singleton<Engine> {
    public:
        Engine();
        virtual ~Engine();

        // Initialization and shutdown
        bool Initialize(const EngineConfig& config = EngineConfig());
        void Shutdown();

        // Main loop control
        void Run();
        void Stop();
        bool IsRunning() const { return m_bRunning; }

        // Frame stepping (for editor integration)
        void Tick(float deltaTime);
        void Render();

        // System accessors (global access to subsystems)
        Window*           GetWindow() const          { return m_pWindow; }
        RenderSystem*     GetRenderer() const        { return m_pRenderer; }
        ResourceManager*  GetResourceManager() const { return m_pResourceManager; }
        InputManager*     GetInput() const           { return m_pInput; }
        AudioSystem*      GetAudio() const           { return m_pAudio; }
        PhysicsSystem*    GetPhysics() const         { return m_pPhysics; }
        AnimationSystem*  GetAnimation() const       { return m_pAnimation; }
        AISystem*         GetAI() const              { return m_pAI; }
        UISystem*         GetUI() const              { return m_pUI; }
        NetworkManager*   GetNetwork() const         { return m_pNetwork; }
        ConsoleSystem*    GetConsole() const         { return m_pConsole; }
        World*            GetWorld() const           { return m_pWorld; }
        Scene*            GetScene() const           { return m_pScene; }
        EventSystem*      GetEvents() const          { return m_pEvents; }
        ConfigManager*    GetConfig() const          { return m_pConfig; }
        FileSystem*       GetFileSystem() const      { return m_pFileSystem; }
        MemoryManager*    GetMemory() const          { return m_pMemory; }
        ThreadPool*       GetThreadPool() const      { return m_pThreadPool; }
        JobSystem*        GetJobSystem() const       { return m_pJobSystem; }
        Profiler*         GetProfiler() const        { return m_pProfiler; }
        DebugDraw*        GetDebugDraw() const       { return m_pDebugDraw; }

        // 1.2 new subsystem accessors
        VideoDecoder*     GetVideoDecoder() const    { return m_pVideoDecoder; }   // 🆕
        MapLoader*        GetMapLoader() const       { return m_pMapLoader; }      // 🆕
        // Forge asset manager (optional – could be separate tool)

        // Engine configuration
        const EngineConfig& GetConfig() const { return m_config; }

        // Time management
        float GetDeltaTime() const      { return m_deltaTime; }
        float GetElapsedTime() const     { return m_elapsedTime; }
        int   GetFrameCount() const      { return m_frameCount; }
        float GetFPS() const             { return m_fps; }

        // Game mode management
        void SetGameMode(GameMode* mode);
        GameMode* GetGameMode() const { return m_pGameMode; }

        // Console commands
        static void RegisterConsoleCommands();

    private:
        // Engine subsystems
        Window*           m_pWindow;
        RenderSystem*     m_pRenderer;
        ResourceManager*  m_pResourceManager;
        InputManager*     m_pInput;
        AudioSystem*      m_pAudio;
        PhysicsSystem*    m_pPhysics;
        AnimationSystem*  m_pAnimation;
        AISystem*         m_pAI;
        UISystem*         m_pUI;
        NetworkManager*   m_pNetwork;
        ConsoleSystem*    m_pConsole;
        World*            m_pWorld;
        Scene*            m_pScene;
        EventSystem*      m_pEvents;
        ConfigManager*    m_pConfig;
        FileSystem*       m_pFileSystem;
        MemoryManager*    m_pMemory;
        ThreadPool*       m_pThreadPool;
        JobSystem*        m_pJobSystem;
        Profiler*         m_pProfiler;
        DebugDraw*        m_pDebugDraw;

        // 1.2 new subsystems
        VideoDecoder*     m_pVideoDecoder;      // 🆕
        MapLoader*        m_pMapLoader;         // 🆕

        // Game mode
        GameMode*         m_pGameMode;

        // Engine configuration
        EngineConfig      m_config;

        // Engine state
        bool              m_bInitialized;
        bool              m_bRunning;

        // Time
        Timer             m_timer;
        float             m_deltaTime;
        float             m_elapsedTime;
        uint64_t          m_frameCount;
        float             m_fps;
        float             m_fpsTimer;

        // Frame limiting
        uint64_t          m_lastFrameTime;
        uint64_t          m_frameTimeTarget;

        // Private methods
        bool CreateSubsystems();
        void DestroySubsystems();
        void CalculateDeltaTime();
        void UpdateFPS();
        void LimitFrameRate();

        // No copying
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;
    };

    // Convenience macro for quick access
    #define USE_ENGINE Engine::Get()

} // namespace USE