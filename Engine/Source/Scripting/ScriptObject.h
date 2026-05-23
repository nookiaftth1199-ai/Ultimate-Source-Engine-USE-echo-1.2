// ============================================================
// Ultimate Source Engine - Script Object
//============================================================
//
// Represents an instance of a script attached to an entity.
// Holds a reference to the script's Lua table (or Python object)
// and provides methods to call functions and access fields.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>

// Forward declaration for Lua state
struct lua_State;

namespace USE {

    class Entity;

    class ScriptObject {
    public:
        // Constructor: creates an empty script object.
        ScriptObject();

        // Destructor: releases the Lua reference.
        ~ScriptObject();

        // Initialize the script object from a Lua table reference.
        // The table is expected at the top of the Lua stack.
        // The Lua state is taken from the scripting system.
        void Initialize(lua_State* L, int tableIndex, Entity* owner);

        // Call a function on the script (with optional arguments).
        bool CallFunction(const std::string& name);
        bool CallFunction(const std::string& name, float arg);
        bool CallFunction(const std::string& name, const std::string& arg);

        // Get a field value from the script (as a string, number, etc.)
        bool GetField(const std::string& name, float& out) const;
        bool GetField(const std::string& name, std::string& out) const;
        bool GetField(const std::string& name, int& out) const;

        // Set a field value in the script.
        void SetField(const std::string& name, float value);
        void SetField(const std::string& name, const std::string& value);
        void SetField(const std::string& name, int value);

        // Check if the script object is valid.
        bool IsValid() const { return m_L != nullptr && m_ref != LUA_NOREF; }

        // Get the owning entity.
        Entity* GetOwner() const { return m_owner; }

    private:
        lua_State* m_L;        // Lua state (owned by scripting system)
        int        m_ref;      // Lua registry reference to the script table
        Entity*    m_owner;    // Entity that owns this script
    };

} // namespace USE