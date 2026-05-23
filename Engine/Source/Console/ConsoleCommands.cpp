// ============================================================
// Ultimate Source Engine - Console Commands Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleSystem.h"
#include "ConsoleCommands.h"
#include "ConsoleVariables.h"
#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/FileSystem.h"
#include "Renderer/RenderSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Network/NetworkManager.h"
#include "Utility/Profiler.h"
#include "Utility/StringUtils.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerController.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace USE {

    // -----------------------------------------------------------------
    // Helper: Get console system (shortcut)
    // -----------------------------------------------------------------
    static ConsoleSystem* Con() {
        return ConsoleSystem::Get();
    }

    // -----------------------------------------------------------------
    // Basic Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_Help(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->Print("============= Ultimate Source Engine Help =============", ConsoleMessageType::System);
            Con()->Print("Commands:", ConsoleMessageType::System);
            
            // List all commands
            for (const auto& pair : Con()->m_commands) {
                const auto& cmd = pair.second;
                std::string line = "  " + cmd.name;
                line += std::string(20 - cmd.name.length(), ' ');
                line += "- " + cmd.description;
                if (cmd.isCheat) line += " [CHEAT]";
                if (cmd.isDevOnly) line += " [DEV]";
                Con()->Print(line, ConsoleMessageType::Info);
            }
            
            Con()->Print("", ConsoleMessageType::System);
            Con()->Print("CVars:", ConsoleMessageType::System);
            Con()->Print("  Type 'cvarlist' to see all console variables", 
                          ConsoleMessageType::Info);
            Con()->Print("==================================================", 
                          ConsoleMessageType::System);
        } else {
            // Help for specific command or CVar
            std::string search = StringUtils::ToLower(args[0]);
            
            // Check commands
            auto cmdIt = Con()->m_commands.find(search);
            if (cmdIt != Con()->m_commands.end()) {
                const auto& cmd = cmdIt->second;
                Con()->Print("Command: " + cmd.name, ConsoleMessageType::System);
                Con()->Print("Description: " + cmd.description, ConsoleMessageType::Info);
                Con()->Print("Usage: " + cmd.usage, ConsoleMessageType::Info);
                if (cmd.isCheat) Con()->Print("Cheat protected", ConsoleMessageType::Warning);
                if (cmd.isDevOnly) Con()->Print("Developer only", ConsoleMessageType::Warning);
                return;
            }
            
            // Check CVars
            auto cvar = Con()->GetCVar(search);
            if (cvar) {
                Con()->Print("CVar: " + cvar->GetName(), ConsoleMessageType::System);
                Con()->Print("Description: " + cvar->GetDescription(), ConsoleMessageType::Info);
                Con()->Print("Type: " + cvar->GetType(), ConsoleMessageType::Info);
                Con()->Print("Current Value: " + cvar->GetString(), ConsoleMessageType::Output);
                Con()->Print("Default Value: " + cvar->GetDefaultString(), ConsoleMessageType::Output);
                return;
            }
            
            Con()->PrintError("No help found for: '" + args[0] + "'");
        }
    }

    void ConsoleCommands::Cmd_Clear(const std::vector<std::string>& args)
    {
        Con()->Clear();
    }

    void ConsoleCommands::Cmd_Echo(const std::vector<std::string>& args)
    {
        std::string text;
        for (const auto& arg : args) {
            text += arg + " ";
        }
        Con()->Print(text, ConsoleMessageType::Info);
    }

    void ConsoleCommands::Cmd_Print(const std::vector<std::string>& args)
    {
        Cmd_Echo(args); // same as echo
    }

    // -----------------------------------------------------------------
    // CVar Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_CVarList(const std::vector<std::string>& args)
    {
        std::string filter;
        if (!args.empty()) filter = StringUtils::ToLower(args[0]);

        Con()->Print("==================================================", ConsoleMessageType::System);
        Con()->Print("Console Variables", ConsoleMessageType::System);
        Con()->Print("==================================================", ConsoleMessageType::System);

        size_t count = 0;
        for (const auto& pair : Con()->m_cvars) {
            if (!filter.empty() && pair.first.find(filter) == std::string::npos)
                continue;
            if (pair.second->HasFlag(CVarFlags::Hidden) && !Con()->GetSettings().developerMode)
                continue;
            
            std::string line = "  " + pair.first;
            line += std::string(25 - pair.first.length(), ' ');
            line += "= " + pair.second->GetString();
            if (pair.second->HasFlag(CVarFlags::ReadOnly)) line += " [READONLY]";
            if (pair.second->HasFlag(CVarFlags::Cheat)) line += " [CHEAT]";
            if (pair.second->HasFlag(CVarFlags::Archive)) line += " [ARCHIVE]";
            Con()->Print(line, ConsoleMessageType::Output);
            count++;
        }
        Con()->Print("Total: " + std::to_string(count) + " CVars", ConsoleMessageType::System);
    }

    void ConsoleCommands::Cmd_CVarDiff(const std::vector<std::string>& args)
    {
        Con()->Print("CVars different from default:", ConsoleMessageType::System);
        int count = 0;
        for (const auto& pair : Con()->m_cvars) {
            if (pair.second->IsModified()) {
                Con()->Print("  " + pair.first + " = " + pair.second->GetString() + 
                             " (default: " + pair.second->GetDefaultString() + ")", 
                             ConsoleMessageType::Output);
                count++;
            }
        }
        if (count == 0) Con()->Print("  None", ConsoleMessageType::Info);
    }

    void ConsoleCommands::Cmd_CVarReset(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: cvarreset <cvar>");
            return;
        }
        std::string name = StringUtils::ToLower(args[0]);
        auto cvar = Con()->GetCVar(name);
        if (!cvar) {
            Con()->PrintError("Unknown CVar: " + name);
            return;
        }
        cvar->ResetToDefault();
        Con()->Print(name + " reset to " + cvar->GetString(), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_CVarDefault(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: cvdefault <cvar>");
            return;
        }
        std::string name = StringUtils::ToLower(args[0]);
        auto cvar = Con()->GetCVar(name);
        if (!cvar) {
            Con()->PrintError("Unknown CVar: " + name);
            return;
        }
        Con()->Print(name + " default = " + cvar->GetDefaultString(), ConsoleMessageType::Output);
    }

    // -----------------------------------------------------------------
    // Command Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_CommandList(const std::vector<std::string>& args)
    {
        std::string filter;
        if (!args.empty()) filter = StringUtils::ToLower(args[0]);

        Con()->Print("==================================================", ConsoleMessageType::System);
        Con()->Print("Console Commands", ConsoleMessageType::System);
        Con()->Print("==================================================", ConsoleMessageType::System);

        size_t count = 0;
        for (const auto& pair : Con()->m_commands) {
            if (!filter.empty() && pair.first.find(filter) == std::string::npos)
                continue;
            std::string line = "  " + pair.first;
            line += std::string(20 - pair.first.length(), ' ');
            line += "- " + pair.second.description;
            if (pair.second.isCheat) line += " [CHEAT]";
            if (pair.second.isDevOnly) line += " [DEV]";
            Con()->Print(line, ConsoleMessageType::Info);
            count++;
        }
        Con()->Print("Total: " + std::to_string(count) + " commands", ConsoleMessageType::System);
    }

    // Simple alias system (store in map)
    static std::map<std::string, std::string> g_aliases;

    void ConsoleCommands::Cmd_Alias(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            Con()->PrintError("Usage: alias <name> <command>");
            return;
        }
        std::string aliasName = StringUtils::ToLower(args[0]);
        std::string command;
        for (size_t i = 1; i < args.size(); ++i) {
            command += args[i] + (i == args.size()-1 ? "" : " ");
        }
        g_aliases[aliasName] = command;
        Con()->Print("Alias created: " + aliasName + " -> " + command, ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_UnAlias(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: unalias <name>");
            return;
        }
        std::string aliasName = StringUtils::ToLower(args[0]);
        if (g_aliases.erase(aliasName)) {
            Con()->Print("Alias removed: " + aliasName, ConsoleMessageType::Success);
        } else {
            Con()->PrintError("Alias not found: " + aliasName);
        }
    }

    // -----------------------------------------------------------------
    // Scripting
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_Exec(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: exec <filename>");
            return;
        }
        Con()->ExecuteFile(args[0]);
    }

    void ConsoleCommands::Cmd_Wait(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: wait <frames>");
            return;
        }
        int frames = std::stoi(args[0]);
        // Simple approach: we'll just block the console? Not good.
        // In a real engine, you'd have a command scheduler. For now, we'll just sleep.
        // But sleeping in console command is bad. We'll ignore.
        Con()->PrintWarning("Wait command not fully implemented; ignoring.");
    }

    void ConsoleCommands::Cmd_If(const std::vector<std::string>& args)
    {
        // Syntax: if <condition> <command>
        if (args.size() < 2) {
            Con()->PrintError("Usage: if <condition> <command>");
            return;
        }
        // Condition parsing: could compare CVars, etc. Too complex for now.
        Con()->PrintWarning("If command not implemented");
    }

    // -----------------------------------------------------------------
    // Console Control
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_ToggleConsole(const std::vector<std::string>& args)
    {
        Con()->Toggle();
    }

    void ConsoleCommands::Cmd_ClearHistory(const std::vector<std::string>& args)
    {
        Con()->m_commandHistory.clear();
        Con()->Print("Command history cleared", ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_SetColorScheme(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: concolor <source|dark|light>");
            return;
        }
        std::string scheme = StringUtils::ToLower(args[0]);
        if (scheme == "source") {
            Con()->SetColorScheme(ConsoleColorScheme::DefaultSourceStyle());
            Con()->Print("Color scheme set to Source", ConsoleMessageType::Success);
        } else if (scheme == "dark") {
            Con()->SetColorScheme(ConsoleColorScheme::DefaultDarkTheme());
            Con()->Print("Color scheme set to Dark", ConsoleMessageType::Success);
        } else if (scheme == "light") {
            Con()->SetColorScheme(ConsoleColorScheme::DefaultLightTheme());
            Con()->Print("Color scheme set to Light", ConsoleMessageType::Success);
        } else {
            Con()->PrintError("Unknown color scheme");
        }
    }

    void ConsoleCommands::Cmd_SetFontSize(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->Print("Current font size: " + std::to_string(Con()->GetSettings().fontSize), ConsoleMessageType::Info);
            return;
        }
        float size = std::stof(args[0]);
        if (size < 6 || size > 72) {
            Con()->PrintError("Font size must be between 6 and 72");
            return;
        }
        Con()->GetSettings().fontSize = size;
        Con()->Print("Font size set to " + std::to_string(size), ConsoleMessageType::Success);
    }

    // -----------------------------------------------------------------
    // Engine Control
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_Quit(const std::vector<std::string>& args)
    {
        Con()->Print("Shutting down Ultimate Source Engine...", ConsoleMessageType::System);
        Engine::Get()->Stop();
    }

    void ConsoleCommands::Cmd_Restart(const std::vector<std::string>& args)
    {
        Con()->Print("Restart not implemented; use quit.", ConsoleMessageType::Warning);
    }

    void ConsoleCommands::Cmd_Status(const std::vector<std::string>& args)
    {
        auto* engine = Engine::Get();
        auto* renderer = engine->GetRenderer();
        auto* physics = engine->GetPhysics();
        auto* network = engine->GetNetwork();

        Con()->Print("==================================================", ConsoleMessageType::System);
        Con()->Print("Ultimate Source Engine Status", ConsoleMessageType::System);
        Con()->Print("==================================================", ConsoleMessageType::System);
        
        Con()->Print("Engine: Ultimate Source Engine v1.0", ConsoleMessageType::Info);
        Con()->Print("Platform: " + std::string(Platform::GetPlatformName()) + " " + 
                     std::string(Platform::GetArchitectureName()), ConsoleMessageType::Info);
        Con()->Print("FPS: " + std::to_string(engine->GetFPS()), ConsoleMessageType::Output);
        Con()->Print("Delta Time: " + std::to_string(engine->GetDeltaTime() * 1000.0f) + " ms", ConsoleMessageType::Output);
        Con()->Print("Frame: " + std::to_string(engine->GetFrameCount()), ConsoleMessageType::Output);
        
        if (renderer) {
            Con()->Print("Renderer: " + std::string(renderer->GetBackendName()), ConsoleMessageType::Output);
            auto stats = renderer->GetStats();
            Con()->Print("  Draw calls: " + std::to_string(stats.drawCalls), ConsoleMessageType::Output);
            Con()->Print("  Triangles: " + std::to_string(stats.trianglesDrawn), ConsoleMessageType::Output);
        }
        
        if (physics) {
            Con()->Print("Physics: " + std::string(physics->GetBackendName()), ConsoleMessageType::Output);
            Con()->Print("  Bodies: " + std::to_string(physics->GetBodyCount()), ConsoleMessageType::Output);
        }
        
        if (network) {
            Con()->Print("Network: " + std::string(network->GetStatusString()), ConsoleMessageType::Output);
            Con()->Print("  Connections: " + std::to_string(network->GetConnectionCount()), ConsoleMessageType::Output);
        }

        Con()->Print("==================================================", ConsoleMessageType::System);
    }

    void ConsoleCommands::Cmd_Version(const std::vector<std::string>& args)
    {
        Con()->Print("Ultimate Source Engine v1.0.0 (Genesis)", ConsoleMessageType::System);
        Con()->Print("Build: " __DATE__ " " __TIME__, ConsoleMessageType::Info);
        Con()->Print("Compiler: " + std::string(Platform::GetCompilerName()), ConsoleMessageType::Info);
    }

    void ConsoleCommands::Cmd_TimeScale(const std::vector<std::string>& args)
    {
        auto* engine = Engine::Get();
        if (args.empty()) {
            Con()->Print("Timescale: " + std::to_string(engine->GetTimeScale()), ConsoleMessageType::Output);
        } else {
            float scale = std::stof(args[0]);
            engine->SetTimeScale(scale);
            Con()->Print("Timescale set to " + std::to_string(scale), ConsoleMessageType::Success);
        }
    }

    // -----------------------------------------------------------------
    // Renderer Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_RenderBackend(const std::vector<std::string>& args)
    {
        auto* engine = Engine::Get();
        auto* renderer = engine->GetRenderer();
        if (args.empty()) {
            Con()->Print("Current render backend: " + std::string(renderer->GetBackendName()), ConsoleMessageType::Output);
            return;
        }

        std::string backend = StringUtils::ToLower(args[0]);
        RenderBackend newBackend;
        if (backend == "opengl") newBackend = RenderBackend::OpenGL;
        else if (backend == "directx9" || backend == "dx9") newBackend = RenderBackend::DirectX9;
        else if (backend == "vulkan") newBackend = RenderBackend::Vulkan;
        else if (backend == "auto") newBackend = RenderBackend::AutoDetect;
        else {
            Con()->PrintError("Unknown backend. Use: opengl, directx9, vulkan, auto");
            return;
        }

        if (engine->GetRenderer()->SetBackend(newBackend)) {
            Con()->Print("Render backend switched to " + std::string(renderer->GetBackendName()), ConsoleMessageType::Success);
        } else {
            Con()->PrintError("Failed to switch backend");
        }
    }

    void ConsoleCommands::Cmd_VSync(const std::vector<std::string>& args)
    {
        auto* renderer = Engine::Get()->GetRenderer();
        if (args.empty()) {
            Con()->Print("VSync: " + std::string(renderer->IsVSyncEnabled() ? "ON" : "OFF"), ConsoleMessageType::Output);
            return;
        }
        bool enable = std::stoi(args[0]) != 0;
        renderer->SetVSync(enable);
        Con()->Print("VSync " + std::string(enable ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Fullscreen(const std::vector<std::string>& args)
    {
        auto* window = Engine::Get()->GetWindow();
        if (args.empty()) {
            Con()->Print("Fullscreen: " + std::string(window->IsFullscreen() ? "ON" : "OFF"), ConsoleMessageType::Output);
            return;
        }
        bool enable = std::stoi(args[0]) != 0;
        window->SetFullscreen(enable);
        Con()->Print("Fullscreen " + std::string(enable ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Screenshot(const std::vector<std::string>& args)
    {
        std::string filename = "screenshot_" + std::to_string(time(nullptr)) + ".bmp";
        if (!args.empty()) filename = args[0];
        Engine::Get()->GetRenderer()->CaptureScreenshot(filename);
        Con()->Print("Screenshot saved as " + filename, ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_ReloadShaders(const std::vector<std::string>& args)
    {
        // This would require shader reloading support in renderer
        Con()->PrintWarning("Shader reload not implemented");
    }

    // -----------------------------------------------------------------
    // Developer Commands (cheats)
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_God(const std::vector<std::string>& args)
    {
        auto* player = Engine::Get()->GetGameMode()->GetLocalPlayerController();
        if (!player) {
            Con()->PrintError("No local player");
            return;
        }
        bool state = !player->IsGodMode(); // toggle
        player->SetGodMode(state);
        Con()->Print("God mode " + std::string(state ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Noclip(const std::vector<std::string>& args)
    {
        auto* player = Engine::Get()->GetGameMode()->GetLocalPlayerController();
        if (!player) {
            Con()->PrintError("No local player");
            return;
        }
        bool state = !player->IsNoClip();
        player->SetNoClip(state);
        Con()->Print("Noclip " + std::string(state ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Give(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: give <item> [amount]");
            return;
        }
        std::string item = args[0];
        int amount = (args.size() > 1) ? std::stoi(args[1]) : 1;
        auto* player = Engine::Get()->GetGameMode()->GetLocalPlayerController();
        if (!player) {
            Con()->PrintError("No local player");
            return;
        }
        player->GiveItem(item, amount);
        Con()->Print("Gave " + std::to_string(amount) + " x " + item, ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Kill(const std::vector<std::string>& args)
    {
        auto* player = Engine::Get()->GetGameMode()->GetLocalPlayerController();
        if (!player) {
            Con()->PrintError("No local player");
            return;
        }
        player->Kill();
        Con()->Print("Player killed", ConsoleMessageType::Warning);
    }

    void ConsoleCommands::Cmd_Spawn(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: spawn <entity> [x y z]");
            return;
        }
        std::string entityClass = args[0];
        Vector3 position;
        if (args.size() >= 4) {
            position.x = std::stof(args[1]);
            position.y = std::stof(args[2]);
            position.z = std::stof(args[3]);
        } else {
            position = Engine::Get()->GetGameMode()->GetLocalPlayerController()->GetPawn()->GetPosition();
        }
        auto* entity = Engine::Get()->GetWorld()->SpawnEntity(entityClass, position);
        if (entity) {
            Con()->Print("Spawned " + entityClass, ConsoleMessageType::Success);
        } else {
            Con()->PrintError("Failed to spawn " + entityClass);
        }
    }

    void ConsoleCommands::Cmd_Teleport(const std::vector<std::string>& args)
    {
        if (args.size() < 3) {
            Con()->PrintError("Usage: teleport <x> <y> <z>");
            return;
        }
        Vector3 pos(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
        auto* player = Engine::Get()->GetGameMode()->GetLocalPlayerController();
        if (!player) {
            Con()->PrintError("No local player");
            return;
        }
        player->GetPawn()->SetPosition(pos);
        Con()->Print("Teleported", ConsoleMessageType::Success);
    }

    // -----------------------------------------------------------------
    // Physics Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_PhysicsDebug(const std::vector<std::string>& args)
    {
        auto* physics = Engine::Get()->GetPhysics();
        bool enable;
        if (args.empty()) {
            // toggle
            enable = !physics->IsDebugDrawingEnabled();
        } else {
            enable = std::stoi(args[0]) != 0;
        }
        physics->SetDebugDrawing(enable);
        Con()->Print("Physics debug drawing " + std::string(enable ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_Gravity(const std::vector<std::string>& args)
    {
        auto* physics = Engine::Get()->GetPhysics();
        if (args.size() < 3) {
            Vector3 g = physics->GetGravity();
            Con()->Print("Gravity: (" + std::to_string(g.x) + ", " + 
                         std::to_string(g.y) + ", " + std::to_string(g.z) + ")", ConsoleMessageType::Output);
            return;
        }
        Vector3 gravity(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
        physics->SetGravity(gravity);
        Con()->Print("Gravity set to (" + std::to_string(gravity.x) + ", " + 
                     std::to_string(gravity.y) + ", " + std::to_string(gravity.z) + ")", ConsoleMessageType::Success);
    }

    // -----------------------------------------------------------------
    // Network Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_Connect(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->PrintError("Usage: connect <address> [port]");
            return;
        }
        std::string address = args[0];
        int port = (args.size() > 1) ? std::stoi(args[1]) : 27015;
        if (Engine::Get()->GetNetwork()->Connect(address, port)) {
            Con()->Print("Connecting to " + address + ":" + std::to_string(port), ConsoleMessageType::Success);
        } else {
            Con()->PrintError("Failed to connect");
        }
    }

    void ConsoleCommands::Cmd_Disconnect(const std::vector<std::string>& args)
    {
        Engine::Get()->GetNetwork()->Disconnect();
        Con()->Print("Disconnected", ConsoleMessageType::Success);
    }

    void ConsoleCommands::Cmd_NetStatus(const std::vector<std::string>& args)
    {
        auto* net = Engine::Get()->GetNetwork();
        Con()->Print("Network status: " + std::string(net->GetStatusString()), ConsoleMessageType::Output);
        Con()->Print("  Ping: " + std::to_string(net->GetPing()) + " ms", ConsoleMessageType::Output);
        Con()->Print("  Bytes sent: " + std::to_string(net->GetBytesSent()), ConsoleMessageType::Output);
        Con()->Print("  Bytes received: " + std::to_string(net->GetBytesReceived()), ConsoleMessageType::Output);
    }

    void ConsoleCommands::Cmd_Ping(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            Con()->Print("Ping: " + std::to_string(Engine::Get()->GetNetwork()->GetPing()) + " ms", ConsoleMessageType::Output);
            return;
        }
        // could ping a specific address
        Con()->PrintWarning("Ping to address not implemented");
    }

    // -----------------------------------------------------------------
    // Benchmark Commands
    // -----------------------------------------------------------------
    void ConsoleCommands::Cmd_Benchmark(const std::vector<std::string>& args)
    {
        int seconds = 10;
        if (!args.empty()) seconds = std::stoi(args[0]);
        Con()->Print("Starting benchmark for " + std::to_string(seconds) + " seconds...", ConsoleMessageType::System);
        Engine::Get()->StartBenchmark(seconds);
    }

    void ConsoleCommands::Cmd_Profile(const std::vector<std::string>& args)
    {
        auto* profiler = Engine::Get()->GetProfiler();
        if (!profiler) {
            Con()->PrintError("Profiler not enabled");
            return;
        }
        if (args.empty()) {
            // toggle
            profiler->SetEnabled(!profiler->IsEnabled());
        } else {
            profiler->SetEnabled(std::stoi(args[0]) != 0);
        }
        Con()->Print("Profiler " + std::string(profiler->IsEnabled() ? "enabled" : "disabled"), ConsoleMessageType::Success);
    }

    // -----------------------------------------------------------------
    // Register all commands (called by ConsoleSystem)
    // -----------------------------------------------------------------
    void ConsoleCommands::RegisterAll(ConsoleSystem* con)
    {
        // Basic
        con->RegisterCommand("help", Cmd_Help,
            "Display help for commands and CVars", "help [command/cvar]");
        con->RegisterCommand("clear", Cmd_Clear,
            "Clear the console screen");
        con->RegisterCommand("echo", Cmd_Echo,
            "Print text to console", "echo <text>");
        con->RegisterCommand("print", Cmd_Print,
            "Print text to console (alias for echo)");

        // CVar commands
        con->RegisterCommand("cvarlist", Cmd_CVarList,
            "List all CVars", "cvarlist [filter]");
        con->RegisterCommand("cvardiff", Cmd_CVarDiff,
            "Show CVars that differ from default");
        con->RegisterCommand("cvarreset", Cmd_CVarReset,
            "Reset a CVar to its default value", "cvarreset <cvar>");
        con->RegisterCommand("cvdefault", Cmd_CVarDefault,
            "Show default value of a CVar", "cvdefault <cvar>");

        // Command commands
        con->RegisterCommand("cmdlist", Cmd_CommandList,
            "List all console commands", "cmdlist [filter]");
        con->RegisterCommand("alias", Cmd_Alias,
            "Create a command alias", "alias <name> <command>");
        con->RegisterCommand("unalias", Cmd_UnAlias,
            "Remove a command alias", "unalias <name>");

        // Scripting
        con->RegisterCommand("exec", Cmd_Exec,
            "Execute a script file", "exec <filename>");
        con->RegisterCommand("wait", Cmd_Wait,
            "Wait for a number of frames (simple)", "wait <frames>");
        con->RegisterCommand("if", Cmd_If,
            "Conditional execution (not fully implemented)", "if <condition> <command>");

        // Console control
        con->RegisterCommand("toggleconsole", Cmd_ToggleConsole,
            "Toggle the console open/closed");
        con->RegisterCommand("clearhistory", Cmd_ClearHistory,
            "Clear command history");
        con->RegisterCommand("concolor", Cmd_SetColorScheme,
            "Set console color scheme (source/dark/light)", "concolor <scheme>");
        con->RegisterCommand("confontsize", Cmd_SetFontSize,
            "Set console font size", "confontsize <size>");

        // Engine control
        con->RegisterCommand("quit", Cmd_Quit,
            "Quit the engine");
        con->RegisterCommand("restart", Cmd_Restart,
            "Restart the engine (not implemented)");
        con->RegisterCommand("status", Cmd_Status,
            "Show engine status");
        con->RegisterCommand("version", Cmd_Version,
            "Show engine version");
        con->RegisterCommand("timescale", Cmd_TimeScale,
            "Set game speed multiplier", "timescale [0.0-4.0]");

        // Renderer commands
        con->RegisterCommand("r_backend", Cmd_RenderBackend,
            "Get or set render backend", "r_backend [opengl|dx9|vulkan|auto]");
        con->RegisterCommand("r_vsync", Cmd_VSync,
            "Toggle vsync", "r_vsync [0/1]");
        con->RegisterCommand("r_fullscreen", Cmd_Fullscreen,
            "Toggle fullscreen mode", "r_fullscreen [0/1]");
        con->RegisterCommand("screenshot", Cmd_Screenshot,
            "Take a screenshot", "screenshot [filename]");
        con->RegisterCommand("reloadshaders", Cmd_ReloadShaders,
            "Reload all shaders (not implemented)");

        // Developer / cheat commands
        con->RegisterCommand("god", Cmd_God,
            "Toggle god mode", "", 0, 0, true, true);
        con->RegisterCommand("noclip", Cmd_Noclip,
            "Toggle noclip mode", "", 0, 0, true, true);
        con->RegisterCommand("give", Cmd_Give,
            "Give item/weapon", "give <item> [amount]", 1, 2, true, true);
        con->RegisterCommand("kill", Cmd_Kill,
            "Kill the player", "", 0, 0, true, true);
        con->RegisterCommand("spawn", Cmd_Spawn,
            "Spawn an entity", "spawn <class> [x y z]", 1, 4, true, true);
        con->RegisterCommand("teleport", Cmd_Teleport,
            "Teleport player to coordinates", "teleport <x> <y> <z>", 3, 3, true, true);

        // Physics
        con->RegisterCommand("physics_debug", Cmd_PhysicsDebug,
            "Toggle physics debug visualization", "physics_debug [0/1]");
        con->RegisterCommand("gravity", Cmd_Gravity,
            "Get or set gravity vector", "gravity [x y z]");

        // Network
        con->RegisterCommand("connect", Cmd_Connect,
            "Connect to a server", "connect <address> [port]");
        con->RegisterCommand("disconnect", Cmd_Disconnect,
            "Disconnect from server");
        con->RegisterCommand("net_status", Cmd_NetStatus,
            "Show network status");
        con->RegisterCommand("ping", Cmd_Ping,
            "Show ping to server", "ping [address]");

        // Benchmark
        con->RegisterCommand("benchmark", Cmd_Benchmark,
            "Run performance benchmark", "benchmark [seconds]");
        con->RegisterCommand("profile", Cmd_Profile,
            "Toggle profiler", "profile [0/1]");
    }

} // namespace USE