// ============================================================
// Ultimate Source Engine - Script Engine Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "ScriptEngine.h"
#include "Core/Logger.h"
#include "Core/FileSystem.h"
#include "Entity/Entity.h"

// Lua includes (if USE_LUA is defined)
#ifdef USE_LUA
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#endif

// Python includes (if USE_PYTHON is defined)
#ifdef USE_PYTHON
#include <Python.h>
#endif

namespace USE {

    // -----------------------------------------------------------------
    // Backend‑specific implementation (Lua)
    // -----------------------------------------------------------------
    #ifdef USE_LUA
    struct LuaScriptObject : public ScriptObject {
        lua_State* L;
        int        ref; // Lua reference to the script table
        Entity*    owner;
    };

    struct ScriptEngine::Impl {
        lua_State* L;
        Impl() : L(nullptr) {}
    };

    bool ScriptEngine::Initialize()
    {
        m_impl = std::make_unique<Impl>();
        m_impl->L = luaL_newstate();
        if (!m_impl->L) {
            USE_LOG_ERROR("ScriptEngine: Failed to create Lua state");
            return false;
        }
        luaL_openlibs(m_impl->L);

        // Register C functions for entity access (optional)
        // For now, we just create a global table for the engine
        lua_newtable(m_impl->L);
        lua_setglobal(m_impl->L, "USE");

        USE_LOG_INFO("ScriptEngine initialized (Lua backend)");
        return true;
    }

    void ScriptEngine::Shutdown()
    {
        if (m_impl && m_impl->L) {
            lua_close(m_impl->L);
            m_impl->L = nullptr;
        }
        m_impl.reset();
    }

    ScriptObject* ScriptEngine::LoadScript(const std::string& filename, Entity* owner)
    {
        if (!m_impl || !m_impl->L) return nullptr;

        // Resolve file path
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ScriptEngine: FileSystem not available");
            return nullptr;
        }
        std::string resolved = fs->ResolvePath(filename);
        if (resolved.empty()) {
            USE_LOG_ERROR("ScriptEngine: Script file not found: %s", filename.c_str());
            return nullptr;
        }

        // Load and execute the script
        if (luaL_dofile(m_impl->L, resolved.c_str()) != LUA_OK) {
            USE_LOG_ERROR("ScriptEngine: Lua error: %s", lua_tostring(m_impl->L, -1));
            lua_pop(m_impl->L, 1);
            return nullptr;
        }

        // The script should return a table (the script object)
        // We assume it returns a table on the stack
        if (!lua_istable(m_impl->L, -1)) {
            USE_LOG_ERROR("ScriptEngine: Script did not return a table (expected script object)");
            lua_pop(m_impl->L, 1);
            return nullptr;
        }

        // Create a reference to the table so it persists
        int ref = luaL_ref(m_impl->L, LUA_REGISTRYINDEX);

        // Create script object
        LuaScriptObject* obj = new LuaScriptObject();
        obj->L = m_impl->L;
        obj->ref = ref;
        obj->owner = owner;

        // Call OnCreate function if it exists
        lua_rawgeti(m_impl->L, LUA_REGISTRYINDEX, ref);
        lua_getfield(m_impl->L, -1, "OnCreate");
        if (lua_isfunction(m_impl->L, -1)) {
            if (lua_pcall(m_impl->L, 0, 0, 0) != LUA_OK) {
                USE_LOG_ERROR("ScriptEngine: OnCreate error: %s", lua_tostring(m_impl->L, -1));
                lua_pop(m_impl->L, 1);
            }
        }
        lua_pop(m_impl->L, 1); // pop the table

        return obj;
    }

    void ScriptEngine::ReleaseScriptObject(ScriptObject* obj)
    {
        if (!obj) return;
        LuaScriptObject* luaObj = static_cast<LuaScriptObject*>(obj);
        if (luaObj->L) {
            // Call OnDestroy
            lua_rawgeti(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
            lua_getfield(luaObj->L, -1, "OnDestroy");
            if (lua_isfunction(luaObj->L, -1)) {
                lua_pcall(luaObj->L, 0, 0, 0);
            }
            lua_pop(luaObj->L, 1);
            // Unref
            luaL_unref(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
        }
        delete luaObj;
    }

    void ScriptEngine::CallFunction(ScriptObject* obj, const std::string& functionName)
    {
        LuaScriptObject* luaObj = static_cast<LuaScriptObject*>(obj);
        if (!luaObj->L) return;
        lua_rawgeti(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
        lua_getfield(luaObj->L, -1, functionName.c_str());
        if (lua_isfunction(luaObj->L, -1)) {
            lua_pcall(luaObj->L, 0, 0, 0);
        }
        lua_pop(luaObj->L, 1);
    }

    void ScriptEngine::CallFunction(ScriptObject* obj, const std::string& functionName, const std::string& arg)
    {
        LuaScriptObject* luaObj = static_cast<LuaScriptObject*>(obj);
        if (!luaObj->L) return;
        lua_rawgeti(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
        lua_getfield(luaObj->L, -1, functionName.c_str());
        if (lua_isfunction(luaObj->L, -1)) {
            lua_pushstring(luaObj->L, arg.c_str());
            lua_pcall(luaObj->L, 1, 0, 0);
        }
        lua_pop(luaObj->L, 1);
    }

    void ScriptEngine::CallFunction(ScriptObject* obj, const std::string& functionName, float deltaTime)
    {
        LuaScriptObject* luaObj = static_cast<LuaScriptObject*>(obj);
        if (!luaObj->L) return;
        lua_rawgeti(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
        lua_getfield(luaObj->L, -1, functionName.c_str());
        if (lua_isfunction(luaObj->L, -1)) {
            lua_pushnumber(luaObj->L, deltaTime);
            lua_pcall(luaObj->L, 1, 0, 0);
        }
        lua_pop(luaObj->L, 1);
    }

    bool ScriptEngine::HasFunction(ScriptObject* obj, const std::string& functionName) const
    {
        LuaScriptObject* luaObj = static_cast<LuaScriptObject*>(obj);
        if (!luaObj->L) return false;
        lua_rawgeti(luaObj->L, LUA_REGISTRYINDEX, luaObj->ref);
        lua_getfield(luaObj->L, -1, functionName.c_str());
        bool has = lua_isfunction(luaObj->L, -1);
        lua_pop(luaObj->L, 2);
        return has;
    }

    #elif defined(USE_PYTHON)
    // Python backend (stub – implement similarly)
    struct ScriptEngine::Impl {
        // Python state
    };
    bool ScriptEngine::Initialize() { USE_LOG_ERROR("Python scripting not implemented"); return false; }
    void ScriptEngine::Shutdown() {}
    ScriptObject* ScriptEngine::LoadScript(const std::string&, Entity*) { return nullptr; }
    void ScriptEngine::ReleaseScriptObject(ScriptObject*) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&, const std::string&) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&, float) {}
    bool ScriptEngine::HasFunction(ScriptObject*, const std::string&) const { return false; }

    #else
    // No scripting backend
    struct ScriptEngine::Impl {};
    bool ScriptEngine::Initialize() { USE_LOG_WARN("ScriptEngine: No backend compiled in"); return false; }
    void ScriptEngine::Shutdown() {}
    ScriptObject* ScriptEngine::LoadScript(const std::string&, Entity*) { return nullptr; }
    void ScriptEngine::ReleaseScriptObject(ScriptObject*) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&, const std::string&) {}
    void ScriptEngine::CallFunction(ScriptObject*, const std::string&, float) {}
    bool ScriptEngine::HasFunction(ScriptObject*, const std::string&) const { return false; }
    #endif

    // -----------------------------------------------------------------
    // Singleton access
    // -----------------------------------------------------------------
    ScriptEngine& ScriptEngine::Get()
    {
        static ScriptEngine instance;
        return instance;
    }

} // namespace USE