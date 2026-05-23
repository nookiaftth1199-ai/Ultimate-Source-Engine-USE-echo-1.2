// ============================================================
// Ultimate Source Engine - Script Object Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ScriptObject.h"
#include "LuaScripting.h"
#include "Core/Logger.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

namespace USE {

    ScriptObject::ScriptObject()
        : m_L(nullptr)
        , m_ref(LUA_NOREF)
        , m_owner(nullptr)
    {
    }

    ScriptObject::~ScriptObject() {
        if (m_L && m_ref != LUA_NOREF) {
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
        }
    }

    void ScriptObject::Initialize(lua_State* L, int tableIndex, Entity* owner) {
        if (m_L && m_ref != LUA_NOREF) {
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
        }
        m_L = L;
        m_owner = owner;

        // Create a reference to the table (so it persists)
        lua_pushvalue(m_L, tableIndex);
        m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

    bool ScriptObject::CallFunction(const std::string& name) {
        if (!m_L || m_ref == LUA_NOREF) return false;

        // Push the script table onto the stack
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        // Get the function field
        lua_getfield(m_L, -1, name.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 2); // pop function and table
            return false;
        }
        // Call the function (0 arguments, 0 results)
        if (lua_pcall(m_L, 0, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptObject: Error calling function '%s': %s",
                          name.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 2); // pop error message and table
            return false;
        }
        lua_pop(m_L, 1); // pop the table
        return true;
    }

    bool ScriptObject::CallFunction(const std::string& name, float arg) {
        if (!m_L || m_ref == LUA_NOREF) return false;

        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_getfield(m_L, -1, name.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 2);
            return false;
        }
        lua_pushnumber(m_L, arg);
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptObject: Error calling function '%s': %s",
                          name.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 2);
            return false;
        }
        lua_pop(m_L, 1);
        return true;
    }

    bool ScriptObject::CallFunction(const std::string& name, const std::string& arg) {
        if (!m_L || m_ref == LUA_NOREF) return false;

        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_getfield(m_L, -1, name.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 2);
            return false;
        }
        lua_pushstring(m_L, arg.c_str());
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("ScriptObject: Error calling function '%s': %s",
                          name.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 2);
            return false;
        }
        lua_pop(m_L, 1);
        return true;
    }

    bool ScriptObject::GetField(const std::string& name, float& out) const {
        if (!m_L || m_ref == LUA_NOREF) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_getfield(m_L, -1, name.c_str());
        if (lua_isnumber(m_L, -1)) {
            out = (float)lua_tonumber(m_L, -1);
            lua_pop(m_L, 2);
            return true;
        }
        lua_pop(m_L, 2);
        return false;
    }

    bool ScriptObject::GetField(const std::string& name, std::string& out) const {
        if (!m_L || m_ref == LUA_NOREF) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_getfield(m_L, -1, name.c_str());
        if (lua_isstring(m_L, -1)) {
            out = lua_tostring(m_L, -1);
            lua_pop(m_L, 2);
            return true;
        }
        lua_pop(m_L, 2);
        return false;
    }

    bool ScriptObject::GetField(const std::string& name, int& out) const {
        if (!m_L || m_ref == LUA_NOREF) return false;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_getfield(m_L, -1, name.c_str());
        if (lua_isinteger(m_L, -1)) {
            out = (int)lua_tointeger(m_L, -1);
            lua_pop(m_L, 2);
            return true;
        }
        lua_pop(m_L, 2);
        return false;
    }

    void ScriptObject::SetField(const std::string& name, float value) {
        if (!m_L || m_ref == LUA_NOREF) return;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushnumber(m_L, value);
        lua_setfield(m_L, -2, name.c_str());
        lua_pop(m_L, 1);
    }

    void ScriptObject::SetField(const std::string& name, const std::string& value) {
        if (!m_L || m_ref == LUA_NOREF) return;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushstring(m_L, value.c_str());
        lua_setfield(m_L, -2, name.c_str());
        lua_pop(m_L, 1);
    }

    void ScriptObject::SetField(const std::string& name, int value) {
        if (!m_L || m_ref == LUA_NOREF) return;
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        lua_pushinteger(m_L, value);
        lua_setfield(m_L, -2, name.c_str());
        lua_pop(m_L, 1);
    }

} // namespace USE