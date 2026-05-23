// ============================================================
// Ultimate Source Engine - Script Function
//============================================================
//
// Represents a callable Lua function. Can be obtained from a
// global function or a table field. Provides methods to call
// the function with different argument types.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

// Forward declaration
struct lua_State;

namespace USE {

    class ScriptFunction {
    public:
        // Create an invalid function.
        ScriptFunction();

        // Create a function from a global Lua function (by name).
        static ScriptFunction FromGlobal(const std::string& name);

        // Create a function from a table field (table must be on stack top).
        // This assumes the table is at the given index and the field name is valid.
        // The function is stored by reference.
        static ScriptFunction FromTable(lua_State* L, int tableIndex, const std::string& fieldName);

        // Copy and move constructors.
        ScriptFunction(const ScriptFunction& other);
        ScriptFunction(ScriptFunction&& other) noexcept;

        // Assignment operators.
        ScriptFunction& operator=(const ScriptFunction& other);
        ScriptFunction& operator=(ScriptFunction&& other) noexcept;

        ~ScriptFunction();

        // Check if the function is valid.
        bool IsValid() const { return m_L != nullptr && m_ref != LUA_NOREF; }
        explicit operator bool() const { return IsValid(); }

        // Call the function with no arguments.
        bool Call();

        // Call with a single float argument.
        bool Call(float arg);

        // Call with a single integer argument.
        bool Call(int arg);

        // Call with a single string argument.
        bool Call(const std::string& arg);

        // Call with a list of arguments (as strings). Returns true if success.
        bool Call(const std::vector<std::string>& args);

        // Call with a variable number of arguments using a format string.
        // Format specifiers: 'f' for float, 'i' for int, 's' for string.
        // Example: Call("fis", 1.5f, 42, "hello");
        bool Call(const char* format, ...);

        // Get the underlying Lua state (for advanced usage).
        lua_State* GetState() const { return m_L; }

    private:
        lua_State* m_L;
        int        m_ref;   // registry reference to the function

        // Internal constructor (does not create a reference)
        ScriptFunction(lua_State* L, int ref);
    };

} // namespace USE