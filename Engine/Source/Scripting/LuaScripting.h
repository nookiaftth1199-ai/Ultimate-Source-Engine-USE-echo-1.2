// ============================================================
// Ultimate Source Engine - Lua Scripting
//============================================================
//
// Provides a high‑level Lua scripting interface.
// Handles loading scripts, calling functions, and binding
// C++ functions to Lua.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

// Lua headers
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace USE {

    class LuaScripting {
    public:
        // Get singleton instance
        static LuaScripting& Get();

        // Initialize Lua state and register built‑in bindings
        bool Initialize();
        void Shutdown();

        // Load and execute a Lua script file. Returns true on success.
        bool DoFile(const std::string& filename);

        // Execute a Lua string (chunk). Returns true on success.
        bool DoString(const std::string& chunk);

        // Get a global variable (as a string) – useful for debugging.
        std::string GetGlobalString(const std::string& name);

        // Call a Lua function with a single float argument (e.g., for update)
        bool CallFunction(const std::string& funcName, float arg);

        // Call a Lua function with a string argument
        bool CallFunction(const std::string& funcName, const std::string& arg);

        // Call a Lua function with no arguments
        bool CallFunction(const std::string& funcName);

        // Get the underlying Lua state (for advanced usage)
        lua_State* GetState() const { return m_L; }

    private:
        LuaScripting();
        ~LuaScripting();

        lua_State* m_L;
        bool m_initialized;

        // Register engine bindings (e.g., print, vector functions)
        void RegisterBindings();
    };

} // namespace USE