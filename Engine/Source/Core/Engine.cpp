// ============================================================
// Ultimate Source Engine - Engine Core Implementation
// Version: 1.2 "Echo"
// ============================================================

#include "stdafx.h"
#include "Engine.h"
#include "Core/Window.h"
#include "Core/SDLWindow.h"
#include "Renderer/RenderSystem.h"
#include "Console/ConsoleCommands.h"
#include "GameFramework/GameMode.h"
#include "Video/VideoDecoder.h"          // 🆕
#include "Resources/MapLoader.h"         // 🆕

#ifdef _WIN32
#include <windows.h>
#endif

namespace USE {

    // -----------------------------------------------------------------
    // Constructor
    // -----------------------------------------------------------------
    Engine::Engine()
        : m_pWindow(nullptr)
        , m_pRenderer(nullptr)
        , m_pResourceManager(nullptr)
        , m_pInput(nullptr)
        , m_pAudio(nullptr)
        , m_pPhysics(nullptr)
        , m_pAnimation(nullptr)
        , m_pAI(nullptr)
        , m_pUI(nullptr)
        , m_pNetwork(nullptr)
        , m_pConsole(nullptr)
        , m_pWorld(nullptr)
        , m_pScene(nullptr)
        , m_pEvents(nullptr)
        , m_pConfig(nullptr)
        , m_pFileSystem(nullptr)
        , m_pMemory(nullptr)
        , m_pThreadPool(nullptr)
        , m_pJobSystem(nullptr)
        , m_pProfiler(nullptr)
        , m_pDebugDraw(nullptr)
        , m_pVideoDecoder(nullptr)       // 🆕
        , m_pMapLoader(nullptr)          // 🆕
        , m_pGameMode(nullptr)
        , m_bInitialized(false)
        , m_bRunning(false)
        , m_deltaTime(0.0f)
        , m_elapsedTime(0.0f)
        , m_frameCount(0)
        , m_fps(0.0f)
        , m_fpsTimer(0.0f)
        , m_lastFrameTime(0)
        , m_frameTimeTarget(0)
    {
        USE_LOG_INFO("Engine instance created.");
    }

    // -----------------------------------------------------------------
    // Destructor
    // -----------------------------------------------------------------
    Engine::~Engine()
    {
        Shutdown();
        USE_LOG_INFO("Engine instance destroyed.");
    }

    // -----------------------------------------------------------------
    // Initialize all engine subsystems
    // -----------------------------------------------------------------
    bool Engine::Initialize(const EngineConfig& config)
    {
        USE_PROFILE_SCOPE("Engine::Initialize");

        if (m_bInitialized) {
            USE_LOG_WARN("Engine already initialized.");
            return true;
        }

        m_config = config;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Ultimate Source Engine 1.2 'Echo' - Initializing");
        USE_LOG_INFO("========================================");
        USE_LOG_INFO("Version: 1.2.0");
        USE_LOG_INFO("Build: " __DATE__ " " __TIME__);
        USE_LOG_INFO("Config: %dx%d %s", config.windowWidth, config.windowHeight,
                     config.fullscreen ? "Fullscreen" : "Windowed");

        // Create all subsystems in correct order
        if (!CreateSubsystems()) {
            USE_LOG_ERROR("Failed to create engine subsystems.");
            return false;
        }

        m_bInitialized = true;
        m_bRunning = true;

        // Register console commands
        RegisterConsoleCommands();

        // Set up frame limiting
        if (m_config.fpsLimit > 0) {
            m_frameTimeTarget = 1000000 / m_config.fpsLimit;
            USE_LOG_INFO("Frame limit: %d FPS", m_config.fpsLimit);
        }

        // Initialize new subsystems
        if (m_config.enableVideoPlayback) {
            m_pVideoDecoder = new VideoDecoder();
            // Optionally pre‑initialize FFmpeg globally (done in VideoDecoder constructor)
        }
        if (m_config.enableMapFormat) {
            m_pMapLoader = new MapLoader();
        }

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine initialization complete.");
        USE_LOG_INFO("========================================");

        return true;
    }

    // -----------------------------------------------------------------
    // Create all subsystems
    // -----------------------------------------------------------------
    bool Engine::CreateSubsystems()
    {
        USE_PROFILE_SCOPE("Engine::CreateSubsystems");

        // 1. Memory manager (must be first)
        m_pMemory = new MemoryManager();
        if (!m_pMemory->Initialize()) {
            USE_LOG_ERROR("Failed to initialize MemoryManager.");
            return false;
        }
        USE_LOG_INFO("MemoryManager initialized.");

        // 2. File system
        m_pFileSystem = new FileSystem();
        if (!m_pFileSystem->Initialize(m_config.assetPath)) {
            USE_LOG_ERROR("Failed to initialize FileSystem.");
            return false;
        }
        USE_LOG_INFO("FileSystem initialized.");

        // 3. Config manager
        m_pConfig = new ConfigManager();
        m_pConfig->Load("engine.cfg");
        USE_LOG_INFO("ConfigManager initialized.");

        // 4. Thread pool
        int threadCount = m_config.threadCount;
        if (threadCount <= 0) {
            threadCount = std::thread::hardware_concurrency();
            if (threadCount == 0) threadCount = 2;
        }
        m_pThreadPool = new ThreadPool(threadCount);
        USE_LOG_INFO("ThreadPool initialized with %d threads.", threadCount);

        // 5. Job system
        m_pJobSystem = new JobSystem();
        m_pJobSystem->Initialize(m_pThreadPool);
        USE_LOG_INFO("JobSystem initialized.");

        // 6. Event system
        m_pEvents = new EventSystem();
        USE_LOG_INFO("EventSystem initialized.");

        // 7. Window
        m_pWindow = new SDLWindow(
            m_config.windowTitle.c_str(),
            m_config.windowWidth,
            m_config.windowHeight
        );
        if (!m_pWindow->Create()) {
            USE_LOG_ERROR("Failed to create window.");
            return false;
        }
        USE_LOG_INFO("Window created: %dx%d", m_config.windowWidth, m_config.windowHeight);

        // 8. Renderer
        m_pRenderer = RenderSystem::Create(
            m_config.renderBackend,
            m_pWindow,
            m_config.vsync
        );
        if (!m_pRenderer || !m_pRenderer->Initialize()) {
            USE_LOG_ERROR("Failed to initialize renderer.");
            return false;
        }
        USE_LOG_INFO("Renderer initialized (%s).", m_pRenderer->GetBackendName());

        // 9. Resource manager
        m_pResourceManager = new ResourceManager();
        if (!m_pResourceManager->Initialize(m_pFileSystem)) {
            USE_LOG_ERROR("Failed to initialize ResourceManager.");
            return false;
        }
        USE_LOG_INFO("ResourceManager initialized.");

        // 10. Input manager
        m_pInput = new InputManager();
        if (!m_pInput->Initialize()) {
            USE_LOG_ERROR("Failed to initialize InputManager.");
            return false;
        }
        USE_LOG_INFO("InputManager initialized.");

        // 11. Audio system
        m_pAudio = new AudioSystem();
        if (!m_pAudio->Initialize()) {
            USE_LOG_WARN("AudioSystem initialization failed (continuing without audio).");
            // Non-fatal
        } else {
            USE_LOG_INFO("AudioSystem initialized.");
        }

        // 12. Physics system
        m_pPhysics = new PhysicsSystem();
        if (!m_pPhysics->Initialize()) {
            USE_LOG_ERROR("Failed to initialize PhysicsSystem.");
            return false;
        }
        USE_LOG_INFO("PhysicsSystem initialized.");

        // 13. Animation system
        m_pAnimation = new AnimationSystem();
        m_pAnimation->Initialize();
        USE_LOG_INFO("AnimationSystem initialized.");

        // 14. AI system
        m_pAI = new AISystem();
        m_pAI->Initialize();
        USE_LOG_INFO("AISystem initialized.");

        // 15. UI system
        m_pUI = new UISystem();
        if (!m_pUI->Initialize(m_pRenderer)) {
            USE_LOG_WARN("UISystem initialization failed (continuing without UI).");
        } else {
            USE_LOG_INFO("UISystem initialized.");
        }

        // 16. Network manager
        m_pNetwork = new NetworkManager();
        if (!m_pNetwork->Initialize()) {
            USE_LOG_WARN("NetworkManager initialization failed (continuing offline).");
        } else {
            USE_LOG_INFO("NetworkManager initialized.");
        }

        // 17. Console system
        if (m_config.enableConsole) {
            m_pConsole = new ConsoleSystem();
            if (!m_pConsole->Initialize(m_pRenderer, m_pWindow)) {
                USE_LOG_WARN("ConsoleSystem initialization failed.");
                delete m_pConsole;
                m_pConsole = nullptr;
            } else {
                USE_LOG_INFO("ConsoleSystem initialized.");
            }
        }

        // 18. Debug draw
        if (m_config.enableDebugDraw) {
            m_pDebugDraw = new DebugDraw();
            m_pDebugDraw->Initialize(m_pRenderer);
            USE_LOG_INFO("DebugDraw initialized.");
        }

        // 19. Profiler
        if (m_config.enableProfiler) {
            m_pProfiler = new Profiler();
            m_pProfiler->Initialize();
            USE_LOG_INFO("Profiler initialized.");
        }

        // 20. World and Scene (empty by default)
        m_pWorld = new World();
        m_pScene = new Scene();

        // 21. 1.2 New subsystems (video decoder, map loader) – created later
        // (They are created in Initialize after subsystems are ready)

        return true;
    }

    // -----------------------------------------------------------------
    // Shutdown all subsystems (reverse order)
    // -----------------------------------------------------------------
    void Engine::Shutdown()
    {
        USE_PROFILE_SCOPE("Engine::Shutdown");

        if (!m_bInitialized) return;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine shutting down...");
        USE_LOG_INFO("========================================");

        // 1.2 new subsystems cleanup
        delete m_pVideoDecoder;
        delete m_pMapLoader;

        // Game mode
        SAFE_DELETE(m_pGameMode);

        // World and Scene
        SAFE_DELETE(m_pScene);
        SAFE_DELETE(m_pWorld);

        // Profiler
        SAFE_DELETE(m_pProfiler);

        // Debug draw
        SAFE_DELETE(m_pDebugDraw);

        // Console
        SAFE_DELETE(m_pConsole);

        // Network
        SAFE_DELETE(m_pNetwork);

        // UI
        SAFE_DELETE(m_pUI);

        // AI
        SAFE_DELETE(m_pAI);

        // Animation
        SAFE_DELETE(m_pAnimation);

        // Physics
        SAFE_DELETE(m_pPhysics);

        // Audio
        SAFE_DELETE(m_pAudio);

        // Input
        SAFE_DELETE(m_pInput);

        // Resource manager
        SAFE_DELETE(m_pResourceManager);

        // Renderer
        SAFE_DELETE(m_pRenderer);

        // Window
        SAFE_DELETE(m_pWindow);

        // Event system
        SAFE_DELETE(m_pEvents);

        // Job system
        SAFE_DELETE(m_pJobSystem);

        // Thread pool
        SAFE_DELETE(m_pThreadPool);

        // Config manager
        SAFE_DELETE(m_pConfig);

        // File system
        SAFE_DELETE(m_pFileSystem);

        // Memory manager (last)
        SAFE_DELETE(m_pMemory);

        m_bInitialized = false;
        m_bRunning = false;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine shutdown complete.");
        USE_LOG_INFO("========================================");
    }

    // -----------------------------------------------------------------
    // Main engine loop
    // -----------------------------------------------------------------
    void Engine::Run()
    {
        USE_PROFILE_SCOPE("Engine::Run");

        if (!m_bInitialized) {
            USE_LOG_ERROR("Engine not initialized, cannot run.");
            return;
        }

        USE_LOG_INFO("Entering main loop...");

        m_timer.Reset();
        m_lastFrameTime = m_timer.GetTicksMicroseconds();
        m_frameCount = 0;
        m_fpsTimer = 0.0f;

        while (m_bRunning) {
            // Calculate delta time
            CalculateDeltaTime();

            // Process OS messages
            m_pWindow->PumpEvents();

            // Tick all systems
            Tick(m_deltaTime);

            // Render
            Render();

            // Update FPS counter
            UpdateFPS();

            // Limit frame rate if needed
            LimitFrameRate();

            m_frameCount++;
        }

        USE_LOG_INFO("Exited main loop.");
    }

    // -----------------------------------------------------------------
    // Stop the engine loop
    // -----------------------------------------------------------------
    void Engine::Stop()
    {
        m_bRunning = false;
        USE_LOG_INFO("Engine stop requested.");
    }

    // -----------------------------------------------------------------
    // Tick all systems (update logic)
    // -----------------------------------------------------------------
    void Engine::Tick(float deltaTime)
    {
        USE_PROFILE_SCOPE("Engine::Tick");

        // Input update
        if (m_pInput) m_pInput->Update();

        // Audio update
        if (m_pAudio) m_pAudio->Update(deltaTime);

        // Physics update
        if (m_pPhysics) m_pPhysics->Update(deltaTime);

        // Animation update
        if (m_pAnimation) m_pAnimation->Update(deltaTime);

        // AI update
        if (m_pAI) m_pAI->Update(deltaTime);

        // World update (entities)
        if (m_pWorld) m_pWorld->Update(deltaTime);

        // Game mode update
        if (m_pGameMode) m_pGameMode->Update(deltaTime);

        // Console update
        if (m_pConsole) m_pConsole->Update(deltaTime);

        // UI update
        if (m_pUI) m_pUI->Update(deltaTime);

        // Network update
        if (m_pNetwork) m_pNetwork->Update(deltaTime);

        // Profiler update
        if (m_pProfiler) m_pProfiler->Update(deltaTime);

        // 1.2 new: Video decoder does not require per‑frame update unless playing video
        // Map loader is used on demand.
    }

    // -----------------------------------------------------------------
    // Render everything
    // -----------------------------------------------------------------
    void Engine::Render()
    {
        USE_PROFILE_SCOPE("Engine::Render");

        if (!m_pRenderer) return;

        // Begin frame
        m_pRenderer->BeginFrame();

        // Clear screen
        m_pRenderer->Clear(CLEAR_COLOR | CLEAR_DEPTH, Color(0.2f, 0.3f, 0.4f, 1.0f));

        // Render scene
        if (m_pScene) m_pScene->Render(m_pRenderer);

        // Render UI
        if (m_pUI) m_pUI->Render();

        // Render debug draw
        if (m_pDebugDraw) m_pDebugDraw->Render(m_pRenderer->GetCamera());

        // Render console on top
        if (m_pConsole) m_pConsole->Render();

        // End frame
        m_pRenderer->EndFrame();
        m_pRenderer->Present();
    }

    // -----------------------------------------------------------------
    // Calculate delta time between frames
    // -----------------------------------------------------------------
    void Engine::CalculateDeltaTime()
    {
        uint64_t currentTime = m_timer.GetTicksMicroseconds();
        m_deltaTime = (currentTime - m_lastFrameTime) / 1000000.0f;

        // Clamp delta time to prevent physics explosion
        if (m_deltaTime > 0.25f) {
            m_deltaTime = 0.25f;
        }

        m_elapsedTime += m_deltaTime;
        m_lastFrameTime = currentTime;
    }

    // -----------------------------------------------------------------
    // Update FPS counter
    // -----------------------------------------------------------------
    void Engine::UpdateFPS()
    {
        m_fpsTimer += m_deltaTime;
        if (m_fpsTimer >= 1.0f) {
            m_fps = static_cast<float>(m_frameCount) / m_fpsTimer;
            m_fpsTimer = 0.0f;
            m_frameCount = 0;

            // Update window title with FPS
            char title[256];
            sprintf_s(title, "%s - FPS: %.1f", m_config.windowTitle.c_str(), m_fps);
            if (m_pWindow) {
                m_pWindow->SetTitle(title);
            }
        }
    }

    // -----------------------------------------------------------------
    // Limit frame rate using busy-wait
    // -----------------------------------------------------------------
    void Engine::LimitFrameRate()
    {
        if (m_frameTimeTarget == 0) return;

        uint64_t currentTime = m_timer.GetTicksMicroseconds();
        uint64_t elapsed = currentTime - m_lastFrameTime;
        if (elapsed < m_frameTimeTarget) {
            uint64_t sleepTime = m_frameTimeTarget - elapsed;
            std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
        }
    }

    // -----------------------------------------------------------------
    // Set the active game mode
    // -----------------------------------------------------------------
    void Engine::SetGameMode(GameMode* mode)
    {
        if (m_pGameMode) {
            m_pGameMode->EndPlay();
            delete m_pGameMode;
        }
        m_pGameMode = mode;
        if (m_pGameMode) {
            m_pGameMode->StartPlay();
        }
    }

    // -----------------------------------------------------------------
    // Register built-in console commands
    // -----------------------------------------------------------------
    void Engine::RegisterConsoleCommands()
    {
        if (!m_pConsole) return;

        // Engine control commands
        m_pConsole->RegisterCommand("quit", [](const std::vector<std::string>&) {
            Engine::Get()->Stop();
        }, "Quit the engine");

        m_pConsole->RegisterCommand("stop", [](const std::vector<std::string>&) {
            Engine::Get()->Stop();
        }, "Stop the engine");

        m_pConsole->RegisterCommand("status", [](const std::vector<std::string>&) {
            auto* engine = Engine::Get();
            engine->GetConsole()->Print("=== Engine Status ===");
            engine->GetConsole()->Print("FPS: %.1f", engine->GetFPS());
            engine->GetConsole()->Print("Frame: %llu", engine->GetFrameCount());
            engine->GetConsole()->Print("Time: %.2f s", engine->GetElapsedTime());
            engine->GetConsole()->Print("Renderer: %s", engine->GetRenderer()->GetBackendName());
            engine->GetConsole()->Print("Resolution: %dx%d", 
                engine->GetWindow()->GetWidth(), 
                engine->GetWindow()->GetHeight());
        }, "Display engine status");

        m_pConsole->RegisterCommand("fps_limit", [](const std::vector<std::string>& args) {
            auto* engine = Engine::Get();
            if (args.empty()) {
                engine->GetConsole()->Print("FPS limit: %d", 
                    engine->GetConfig().fpsLimit);
            } else {
                int limit = std::stoi(args[0]);
                engine->m_config.fpsLimit = limit;
                if (limit > 0) {
                    engine->m_frameTimeTarget = 1000000 / limit;
                } else {
                    engine->m_frameTimeTarget = 0;
                }
                engine->GetConsole()->Print("FPS limit set to %d", limit);
            }
        }, "Set FPS limit", "fps_limit [0=unlimited]");

        m_pConsole->RegisterCommand("vsync", [](const std::vector<std::string>& args) {
            auto* engine = Engine::Get();
            if (args.empty()) {
                engine->GetConsole()->Print("VSync: %s", 
                    engine->GetRenderer()->IsVSyncEnabled() ? "ON" : "OFF");
            } else {
                bool enable = std::stoi(args[0]) != 0;
                engine->GetRenderer()->SetVSync(enable);
                engine->GetConsole()->Print("VSync %s", enable ? "enabled" : "disabled");
            }
        }, "Toggle vertical sync", "vsync [0/1]");

        // Developer commands
        if (m_config.enableConsole) {
            m_pConsole->RegisterCommand("god", [](const std::vector<std::string>&) {
                // Implement god mode
                Engine::Get()->GetConsole()->Print("God mode toggled (not implemented)");
            }, "Toggle god mode", "", 0, 0, true);

            m_pConsole->RegisterCommand("noclip", [](const std::vector<std::string>&) {
                Engine::Get()->GetConsole()->Print("Noclip toggled (not implemented)");
            }, "Toggle noclip", "", 0, 0, true);
        }
    }

} // namespace USE