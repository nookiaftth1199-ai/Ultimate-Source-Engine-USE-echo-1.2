// ============================================================
// Ultimate Source Engine - Script Function Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ScriptFunction.h"
#include "LuaScripting.h"
#include "Core/Logger.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace USE {

    // -----------------------------------------------------------------
    // Helper: push arguments based on format string (va_list version)
    // -----------------------------------------------------------------
    static int PushArgs(lua_State* L, const char* format, va_list args) {
        int count = 0;
        for (const char* p = format; *p; ++p) {
            switch (*p) {
                case 'f':
                    lua_pushnumber(L, va_arg(args, double));
                    ++count;
                    break;
                case 'i':
                    lua_pushinteger(L, va_arg(args, int));
                    ++count;
                    break;
                case 's':
                    lua_pushstring(L, va_arg(args, const char*));
                    ++count;
                    break;
                default:
                    break;
            }
        }
        return count;
    }

    // -----------------------------------------------------------------
    // Constructors / Destructor
    // -----------------------------------------------------------------
    ScriptFunction::ScriptFunction()
        : m_L(nullptr)
        , m_ref(LUA_NOREF)
    {
    }

    ScriptFunction::ScriptFunction(lua_State* L, int ref)
        : m_L(L)
        , m_ref(ref)
    {
    }

    ScriptFunction::ScriptFunction(const ScriptFunction& other)
        : m_L(other.m_L)
        , m_ref(LUA_NOREF)
    {
        if (other.m_L && other.m_ref != LUA_NOREF) {
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, other.m_ref);
            m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
        }
    }

    ScriptFunction::ScriptFunction(ScriptFunction&& other) noexcept
        : m_L(other.m_L)
        , m_ref(other.m_ref)
    {
        other.m_L = nullptr;
        other.m_ref = LUA_NOREF;
    }

    ScriptFunction& ScriptFunction::operator=(const ScriptFunction& other) {
        if (this != &other) {
            // Release current reference
            if (m_L && m_ref != LUA_NOREF) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_L = other.m_L;
            if (other.m_L && other.m_ref != LUA_NOREF) {
                lua_rawgeti(m_L, LUA_REGISTRYINDEX, other.m_ref);
                m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
            } else {
                m_ref = LUA_NOREF;
            }
        }
        return *this;
    }

    ScriptFunction& ScriptFunction::operator=(ScriptFunction&& other) noexcept {
        if (this != &other) {
            if (m_L && m_ref != LUA_NOREF) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_L = other.m_L;
            m_ref = other.m_ref;
            other.m_L = nullptr;
            other.m_ref = LUA_NOREF;
        }
        return *this;
    }

    ScriptFunction::~ScriptFunction() {
        if (m_L && m_ref != LUA_NOREF) {
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
        }
    }

    // -----------------------------------------------------------------
    // Factory methods
    // -----------------------------------------------------------------
    ScriptFunction ScriptFunction::FromGlobal(const std::string& name) {
        lua_State* L = LuaScripting::Get().GetState();
        if (!L) return ScriptFunction();

        lua_getglobal(L, name.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            USE_LOG_WARN("ScriptFunction: Global '%s' is not a function", name.c_str());
            return ScriptFunction();
        }
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        return ScriptFunction(L, ref);
    }

    ScriptFunction ScriptFunction::FromTable(lua_State* L, int tableIndex, const std::string& fieldName) {
        if (!L) return ScriptFunction();
        lua_pushstring(L, fieldName.c_str());
        lua_gettable(L, tableIndex);
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 1);
            USE_LOG_WARN("ScriptFunction: Field '%s' is not a function", fieldName.c_str());
            return ScriptFunction();
        }
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        return ScriptFunction(L, ref);
    }

    // -----------------------------------------------------------------
    // Call methods
    // -----------------------------------------------------------------
    bool ScriptFunction::Call() {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        if (lua_pcall(m_L, 0, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool ScriptFunction::Call(float arg) {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushnumber(m_L, arg);
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool ScriptFunction::Call(int arg) {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushinteger(m_L, arg);
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool ScriptFunction::Call(const std::string& arg) {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushstring(m_L, arg.c_str());
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool ScriptFunction::Call(const std::vector<std::string>& args) {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        for (const auto& arg : args) {
            lua_pushstring(m_L, arg.c_str());
        }
        int nargs = (int)args.size();
        if (lua_pcall(m_L, nargs, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool ScriptFunction::Call(const char* format, ...) {
        if (!IsValid()) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        va_list args;
        va_start(args, format);
        int nargs = PushArgs(m_L, format, args);
        va_end(args);
        if (lua_pcall(m_L, nargs, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptFunction: Error calling function: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

} // namespace USE