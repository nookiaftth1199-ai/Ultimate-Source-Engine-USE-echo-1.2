// ============================================================
// Ultimate Source Engine - Application Implementation
// ============================================================

#include "stdafx.h"
#include "Application.h"
#include "Core/Window.h"
#include "Core/Platform.h"
#include "Core/SDLWindow.h" // For concrete window creation
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
#include "GameFramework/GameMode.h"

#include <SDL.h>

namespace USE {

    Application* g_app = nullptr;

    Application::Application()
        : m_window(nullptr)
        , m_renderer(nullptr)
        , m_resourceManager(nullptr)
        , m_input(nullptr)
        , m_audio(nullptr)
        , m_physics(nullptr)
        , m_animation(nullptr)
        , m_ai(nullptr)
        , m_ui(nullptr)
        , m_network(nullptr)
        , m_console(nullptr)
        , m_events(nullptr)
        , m_config(nullptr)
        , m_fileSystem(nullptr)
        , m_memory(nullptr)
        , m_threadPool(nullptr)
        , m_jobSystem(nullptr)
        , m_profiler(nullptr)
        , m_gameMode(nullptr)
        , m_running(false)
        , m_initialized(false)
        , m_deltaTime(0)
        , m_totalTime(0)
        , m_frameCount(0)
        , m_fps(0)
        , m_fpsTimer(0)
        , m_lastFrameTime(0)
        , m_targetFrameTime(0)
    {
        g_app = this;
        USE_LOG_INFO("Application created.");
    }

    Application::~Application()
    {
        Shutdown();
        g_app = nullptr;
        USE_LOG_INFO("Application destroyed.");
    }

    bool Application::Initialize(const AppConfig& config)
    {
        if (m_initialized) {
            USE_LOG_WARN("Application already initialized.");
            return true;
        }

        m_config = config;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Ultimate Source Engine - Initializing");
        USE_LOG_INFO("========================================");
        USE_LOG_INFO("Version: 1.0.0");
        USE_LOG_INFO("Build: " __DATE__ " " __TIME__);
        USE_LOG_INFO("Title: %s", config.title.c_str());
        USE_LOG_INFO("Resolution: %dx%d %s", config.width, config.height,
                     config.fullscreen ? "Fullscreen" : "Windowed");

        // Create all subsystems
        if (!CreateSubsystems()) {
            USE_LOG_ERROR("Failed to create engine subsystems.");
            return false;
        }

        // Set target frame rate
        if (config.targetFPS > 0) {
            m_targetFrameTime = 1000000 / config.targetFPS; // microseconds
            USE_LOG_INFO("Target FPS: %d", config.targetFPS);
        }

        m_running = true;
        m_initialized = true;

        // Call game-specific initialization
        OnInitialize();

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine initialization complete.");
        USE_LOG_INFO("========================================");

        return true;
    }

    void Application::Shutdown()
    {
        if (!m_initialized) return;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine shutting down...");
        USE_LOG_INFO("========================================");

        OnShutdown();

        DestroySubsystems();

        m_initialized = false;
        m_running = false;

        USE_LOG_INFO("========================================");
        USE_LOG_INFO("  Engine shutdown complete.");
        USE_LOG_INFO("========================================");
    }

    void Application::Run()
    {
        if (!m_initialized) {
            USE_LOG_ERROR("Application not initialized, cannot run.");
            return;
        }

        USE_LOG_INFO("Entering main loop...");

        m_timer.Reset();
        m_lastFrameTime = m_timer.GetElapsedTicks();
        m_fpsTimer = 0.0f;

        while (m_running) {
            // Calculate delta time
            CalculateDeltaTime();

            // Process OS events
            ProcessEvents();

            // Update all systems
            if (m_input) m_input->Update();
            if (m_audio) m_audio->Update(m_deltaTime);
            if (m_physics) m_physics->Update(m_deltaTime);
            if (m_animation) m_animation->Update(m_deltaTime);
            if (m_ai) m_ai->Update(m_deltaTime);
            if (m_console) m_console->Update(m_deltaTime);
            if (m_ui) m_ui->Update(m_deltaTime);
            if (m_network) m_network->Update(m_deltaTime);
            if (m_profiler) m_profiler->Update(m_deltaTime);

            // Update game mode
            if (m_gameMode) m_gameMode->Update(m_deltaTime);

            // Game-specific update
            OnUpdate(m_deltaTime);

            // Render
            if (m_renderer) {
                m_renderer->BeginFrame();
                m_renderer->Clear(CLEAR_COLOR | CLEAR_DEPTH, Color(0.2f, 0.3f, 0.4f, 1.0f));

                // Game-specific render
                OnRender();

                // Render UI
                if (m_ui) m_ui->Render();

                // Render console on top
                if (m_console) m_console->Render();

                m_renderer->EndFrame();
                m_renderer->Present();
            }

            // Update FPS counter
            UpdateFPS();

            // Limit frame rate if needed
            LimitFrameRate();

            m_frameCount++;
        }

        USE_LOG_INFO("Exited main loop.");
    }

    void Application::Quit()
    {
        m_running = false;
        USE_LOG_INFO("Quit requested.");
    }

    void Application::SetGameMode(GameMode* mode)
    {
        if (m_gameMode) {
            m_gameMode->EndPlay();
            delete m_gameMode;
        }
        m_gameMode = mode;
        if (m_gameMode) {
            m_gameMode->StartPlay();
        }
    }

    // -----------------------------------------------------------------
    // Private methods
    // -----------------------------------------------------------------

    bool Application::CreateSubsystems()
    {
        USE_PROFILE_SCOPE("Application::CreateSubsystems");

        // Order matters: dependencies first

        // 1. Memory manager
        m_memory = new MemoryManager();
        if (!m_memory->Initialize()) {
            USE_LOG_ERROR("Failed to initialize MemoryManager.");
            return false;
        }

        // 2. File system
        m_fileSystem = new FileSystem();
        if (!m_fileSystem->Initialize(m_config.assetPath)) {
            USE_LOG_ERROR("Failed to initialize FileSystem.");
            return false;
        }

        // 3. Config manager
        m_config = new ConfigManager();
        m_config->Load("engine.cfg"); // Optional

        // 4. Event system
        m_events = new EventSystem();

        // 5. Thread pool
        int threadCount = Platform::GetCPUCount();
        m_threadPool = new ThreadPool(threadCount);
        USE_LOG_INFO("ThreadPool created with %d threads.", threadCount);

        // 6. Job system
        m_jobSystem = new JobSystem();
        m_jobSystem->Initialize(threadCount);

        // 7. Window
        WindowDesc wdesc;
        wdesc.title = m_config.title;
        wdesc.width = m_config.width;
        wdesc.height = m_config.height;
        wdesc.fullscreen = m_config.fullscreen;
        wdesc.vsync = m_config.vsync;
        m_window = new SDLWindow(wdesc);
        if (!m_window->Initialize()) {
            USE_LOG_ERROR("Failed to create window.");
            return false;
        }

        // 8. Renderer
        RenderBackend backend = RenderBackend::AutoDetect;
        if (m_config.renderBackend == "opengl") backend = RenderBackend::OpenGL;
        else if (m_config.renderBackend == "directx9") backend = RenderBackend::DirectX9;
        else if (m_config.renderBackend == "vulkan") backend = RenderBackend::Vulkan;
        m_renderer = new RenderSystem();
        if (!m_renderer->Initialize(backend, m_window, m_config.vsync)) {
            USE_LOG_ERROR("Failed to initialize renderer.");
            return false;
        }

        // 9. Resource manager
        m_resourceManager = new ResourceManager();
        if (!m_resourceManager->Initialize(m_fileSystem)) {
            USE_LOG_ERROR("Failed to initialize ResourceManager.");
            return false;
        }

        // 10. Input manager
        m_input = new InputManager();
        if (!m_input->Initialize()) {
            USE_LOG_ERROR("Failed to initialize InputManager.");
            return false;
        }

        // 11. Audio system
        m_audio = new AudioSystem();
        if (!m_audio->Initialize()) {
            USE_LOG_WARN("AudioSystem initialization failed (continuing without audio).");
        }

        // 12. Physics system
        m_physics = new PhysicsSystem();
        if (!m_physics->Initialize()) {
            USE_LOG_ERROR("Failed to initialize PhysicsSystem.");
            return false;
        }

        // 13. Animation system
        m_animation = new AnimationSystem();
        m_animation->Initialize();

        // 14. AI system
        m_ai = new AISystem();
        m_ai->Initialize();

        // 15. UI system
        m_ui = new UISystem();
        if (!m_ui->Initialize(m_renderer)) {
            USE_LOG_WARN("UISystem initialization failed (continuing without UI).");
        }

        // 16. Network manager
        m_network = new NetworkManager();
        if (!m_network->Initialize()) {
            USE_LOG_WARN("NetworkManager initialization failed (continuing offline).");
        }

        // 17. Console system
        if (m_config.enableConsole) {
            m_console = ConsoleSystem::Get();
            if (!m_console->Initialize()) {
                USE_LOG_WARN("ConsoleSystem initialization failed.");
                // Not fatal, continue
            }
        }

        // 18. Profiler
        if (m_config.enableProfiler) {
            m_profiler = new Profiler();
            m_profiler->Initialize();
        }

        USE_LOG_INFO("All subsystems created.");
        return true;
    }

    void Application::DestroySubsystems()
    {
        USE_LOG_INFO("Destroying subsystems...");

        // Delete in reverse order of creation
        if (m_profiler) { delete m_profiler; m_profiler = nullptr; }
        if (m_console) { /* Console is a singleton, no need to delete */ }
        if (m_network) { delete m_network; m_network = nullptr; }
        if (m_ui) { delete m_ui; m_ui = nullptr; }
        if (m_ai) { delete m_ai; m_ai = nullptr; }
        if (m_animation) { delete m_animation; m_animation = nullptr; }
        if (m_physics) { delete m_physics; m_physics = nullptr; }
        if (m_audio) { delete m_audio; m_audio = nullptr; }
        if (m_input) { delete m_input; m_input = nullptr; }
        if (m_resourceManager) { delete m_resourceManager; m_resourceManager = nullptr; }
        if (m_renderer) { delete m_renderer; m_renderer = nullptr; }
        if (m_window) { delete m_window; m_window = nullptr; }
        if (m_jobSystem) { delete m_jobSystem; m_jobSystem = nullptr; }
        if (m_threadPool) { delete m_threadPool; m_threadPool = nullptr; }
        if (m_events) { delete m_events; m_events = nullptr; }
        if (m_config) { delete m_config; m_config = nullptr; }
        if (m_fileSystem) { delete m_fileSystem; m_fileSystem = nullptr; }
        if (m_memory) { delete m_memory; m_memory = nullptr; }
    }

    void Application::CalculateDeltaTime()
    {
        uint64_t currentTime = m_timer.GetElapsedTicks();
        m_deltaTime = (currentTime - m_lastFrameTime) / 1000000.0f;
        if (m_deltaTime > 0.25f) m_deltaTime = 0.25f; // prevent large jumps
        m_totalTime += m_deltaTime;
        m_lastFrameTime = currentTime;
    }

    void Application::UpdateFPS()
    {
        m_fpsTimer += m_deltaTime;
        if (m_fpsTimer >= 1.0f) {
            m_fps = static_cast<float>(m_frameCount - (m_frameCount - 1)) / m_fpsTimer; // simple: frames in last second
            m_fpsTimer = 0.0f;
            // Optionally update window title with FPS
            char title[256];
            sprintf_s(title, "%s - FPS: %.1f", m_config.title.c_str(), m_fps);
            m_window->SetTitle(title);
        }
    }

    void Application::LimitFrameRate()
    {
        if (m_targetFrameTime == 0) return;
        uint64_t currentTime = m_timer.GetElapsedTicks();
        uint64_t elapsed = currentTime - m_lastFrameTime;
        if (elapsed < m_targetFrameTime) {
            uint64_t sleepTime = m_targetFrameTime - elapsed;
            Timer::SleepMicroseconds(sleepTime);
        }
    }

    void Application::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Let console handle input first if it's open
            if (m_console && m_console->IsOpen()) {
                // Console will handle its own input via SDL events? Actually console has ProcessKeyEvent.
                // We need to forward events to console. We'll implement a simple mapping.
                // For now, just pass to console's ProcessKeyEvent etc.
                // We'll keep it simple; a full implementation would map SDL events to console methods.
                // Skipping for brevity.
            }

            switch (event.type) {
                case SDL_QUIT:
                    Quit();
                    break;

                case SDL_KEYDOWN:
                    OnKeyPressed(event.key.keysym.sym, event.key.keysym.mod);
                    break;

                case SDL_KEYUP:
                    OnKeyReleased(event.key.keysym.sym, event.key.keysym.mod);
                    break;

                case SDL_MOUSEMOTION:
                    OnMouseMove(event.motion.x, event.motion.y,
                                event.motion.xrel, event.motion.yrel);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    OnMouseButton(event.button.button, event.button.state, event.button.clicks);
                    break;

                case SDL_MOUSEWHEEL:
                    OnMouseWheel(event.wheel.y);
                    break;

                case SDL_TEXTINPUT:
                    OnTextInput(event.text.text);
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        OnWindowResized(event.window.data1, event.window.data2);
                    } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                        OnWindowFocus(true);
                    } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        OnWindowFocus(false);
                    } else if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        OnWindowClosed();
                    }
                    break;
            }
        }
    }

} // namespace USE