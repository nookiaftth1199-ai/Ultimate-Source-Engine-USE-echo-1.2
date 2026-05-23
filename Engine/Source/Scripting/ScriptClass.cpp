// ============================================================
// Ultimate Source Engine - Script Class Binding Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ScriptClass.h"
#include "LuaScripting.h"

namespace USE {

    void ScriptClass::RegisterClass(const char* name,
                                     void* (*createFunc)(lua_State*),
                                     const luaL_Reg* methods)
    {
        lua_State* L = LuaScripting::Get().GetState();
        if (!L) return;

        // Create a new metatable for the class
        luaL_newmetatable(L, name);
        // Set __index to itself
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");

        // Register methods
        luaL_setfuncs(L, methods, 0);

        // Register constructor (as a global function)
        lua_pushcfunction(L, createFunc);
        lua_setglobal(L, name);
    }

    // Default implementation does nothing; derived classes should override
    void ScriptClass::RegisterAll() {
        // No‑op; subclasses should call RegisterClass for their own types.
    }

} // namespace USE