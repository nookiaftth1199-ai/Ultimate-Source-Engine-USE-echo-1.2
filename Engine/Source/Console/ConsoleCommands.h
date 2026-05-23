// ============================================================
// Ultimate Source Engine - Console Commands
// Version: 1.0.0
// ============================================================

#pragma once
#include "../stdafx.h"
#include "ConsoleSystem.h"

namespace USE {

    // -----------------------------------------------------------------
    // Console Commands System
    // -----------------------------------------------------------------
    class ConsoleCommands {
    public:
        static void RegisterAll(ConsoleSystem* pConsole);

    private:
        // Basic commands
        static void Cmd_Help(const std::vector<std::string>& args);
        static void Cmd_Clear(const std::vector<std::string>& args);
        static void Cmd_Echo(const std::vector<std::string>& args);
        static void Cmd_Print(const std::vector<std::string>& args);
        
        // CVar commands
        static void Cmd_CVarList(const std::vector<std::string>& args);
        static void Cmd_CVarDiff(const std::vector<std::string>& args);
        static void Cmd_CVarReset(const std::vector<std::string>& args);
        static void Cmd_CVarDefault(const std::vector<std::string>& args);
        
        // Command commands
        static void Cmd_CommandList(const std::vector<std::string>& args);
        static void Cmd_Alias(const std::vector<std::string>& args);
        static void Cmd_UnAlias(const std::vector<std::string>& args);
        
        // Scripting
        static void Cmd_Exec(const std::vector<std::string>& args);
        static void Cmd_Wait(const std::vector<std::string>& args);
        static void Cmd_If(const std::vector<std::string>& args);
        
        // Console control
        static void Cmd_ToggleConsole(const std::vector<std::string>& args);
        static void Cmd_ClearHistory(const std::vector<std::string>& args);
        static void Cmd_SetColorScheme(const std::vector<std::string>& args);
        static void Cmd_SetFontSize(const std::vector<std::string>& args);
        
        // Engine control
        static void Cmd_Quit(const std::vector<std::string>& args);
        static void Cmd_Restart(const std::vector<std::string>& args);
        static void Cmd_Status(const std::vector<std::string>& args);
        static void Cmd_Version(const std::vector<std::string>& args);
        static void Cmd_TimeScale(const std::vector<std::string>& args);
        
        // Renderer commands
        static void Cmd_RenderBackend(const std::vector<std::string>& args);
        static void Cmd_VSync(const std::vector<std::string>& args);
        static void Cmd_Fullscreen(const std::vector<std::string>& args);
        static void Cmd_Screenshot(const std::vector<std::string>& args);
        static void Cmd_ReloadShaders(const std::vector<std::string>& args);
        
        // Developer commands
        static void Cmd_God(const std::vector<std::string>& args);
        static void Cmd_Noclip(const std::vector<std::string>& args);
        static void Cmd_Give(const std::vector<std::string>& args);
        static void Cmd_Kill(const std::vector<std::string>& args);
        static void Cmd_Spawn(const std::vector<std::string>& args);
        static void Cmd_Teleport(const std::vector<std::string>& args);
        
        // Physics commands
        static void Cmd_PhysicsDebug(const std::vector<std::string>& args);
        static void Cmd_Gravity(const std::vector<std::string>& args);
        
        // Network commands
        static void Cmd_Connect(const std::vector<std::string>& args);
        static void Cmd_Disconnect(const std::vector<std::string>& args);
        static void Cmd_Status(const std::vector<std::string>& args);
        static void Cmd_Ping(const std::vector<std::string>& args);
        
        // Benchmark
        static void Cmd_Benchmark(const std::vector<std::string>& args);
        static void Cmd_Profile(const std::vector<std::string>& args);
    };

} // namespace USE