// ============================================================
// Ultimate Source Engine - Script Class Binding
//============================================================
//
// Provides a way to expose C++ classes to Lua (or other scripting
// languages) with methods and properties.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace USE {

    // Forward declarations
    class ScriptObject;

    // Base class for script‑bound classes
    class ScriptClass {
    public:
        virtual ~ScriptClass() = default;

        // Get the name of the class as exposed to the script.
        virtual const char* GetClassName() const = 0;

        // Register the class with the scripting system (Lua).
        // This is typically called once during engine initialization.
        static void RegisterAll();

    protected:
        // Helper to register a class with Lua.
        // The 'name' is the global table name (e.g., "Vector3").
        // 'createFunc' is a function that returns a new instance (as a userdata).
        // 'methods' is a table mapping method names to C functions.
        static void RegisterClass(const char* name,
                                   void* (*createFunc)(lua_State*),
                                   const luaL_Reg* methods);
    };

} // namespace USE