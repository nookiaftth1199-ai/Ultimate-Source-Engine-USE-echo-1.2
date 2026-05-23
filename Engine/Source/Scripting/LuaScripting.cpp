// ============================================================
// Ultimate Source Engine - Lua Scripting Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "LuaScripting.h"
#include "Core/Logger.h"
#include "Math/Vector3.h"

namespace USE {

    // -----------------------------------------------------------------
    // Helper: print function for Lua
    // -----------------------------------------------------------------
    static int lua_print(lua_State* L) {
        int n = lua_gettop(L);
        for (int i = 1; i <= n; ++i) {
            if (i > 1) printf("\t");
            if (lua_isstring(L, i)) {
                printf("%s", lua_tostring(L, i));
            } else {
                printf("%s", lua_typename(L, lua_type(L, i)));
            }
        }
        printf("\n");
        return 0;
    }

    // -----------------------------------------------------------------
    // Helper: create a Vector3 table
    // -----------------------------------------------------------------
    static int lua_vector3_new(lua_State* L) {
        float x = (float)luaL_optnumber(L, 1, 0.0);
        float y = (float)luaL_optnumber(L, 2, 0.0);
        float z = (float)luaL_optnumber(L, 3, 0.0);
        lua_newtable(L);
        lua_pushnumber(L, x); lua_setfield(L, -2, "x");
        lua_pushnumber(L, y); lua_setfield(L, -2, "y");
        lua_pushnumber(L, z); lua_setfield(L, -2, "z");
        // Add a method to get length
        lua_pushcfunction(L, [](lua_State* L2) -> int {
            lua_getfield(L2, 1, "x");
            lua_getfield(L2, 1, "y");
            lua_getfield(L2, 1, "z");
            float x = (float)lua_tonumber(L2, -3);
            float y = (float)lua_tonumber(L2, -2);
            float z = (float)lua_tonumber(L2, -1);
            lua_pop(L2, 3);
            float len = sqrtf(x*x + y*y + z*z);
            lua_pushnumber(L2, len);
            return 1;
        });
        lua_setfield(L, -2, "length");
        return 1;
    }

    // -----------------------------------------------------------------
    // Singleton
    // -----------------------------------------------------------------
    LuaScripting& LuaScripting::Get() {
        static LuaScripting instance;
        return instance;
    }

    LuaScripting::LuaScripting()
        : m_L(nullptr)
        , m_initialized(false)
    {
    }

    LuaScripting::~LuaScripting() {
        Shutdown();
    }

    bool LuaScripting::Initialize() {
        if (m_initialized) return true;

        m_L = luaL_newstate();
        if (!m_L) {
            USE_LOG_ERROR("LuaScripting: Failed to create Lua state");
            return false;
        }

        luaL_openlibs(m_L);
        RegisterBindings();

        m_initialized = true;
        USE_LOG_INFO("LuaScripting initialized");
        return true;
    }

    void LuaScripting::Shutdown() {
        if (m_L) {
            lua_close(m_L);
            m_L = nullptr;
        }
        m_initialized = false;
    }

    void LuaScripting::RegisterBindings() {
        // Register print
        lua_register(m_L, "print", lua_print);

        // Register Vector3 constructor
        lua_register(m_L, "Vector3", lua_vector3_new);

        // Optional: register more engine functions (e.g., GetEntity, SetPosition, etc.)
        // This is where you'd expose your game API to Lua.
    }

    bool LuaScripting::DoFile(const std::string& filename) {
        if (!m_initialized) return false;

        // Resolve path using FileSystem (if available)
        FileSystem* fs = FileSystem::Get();
        std::string resolved = filename;
        if (fs) {
            resolved = fs->ResolvePath(filename);
            if (resolved.empty()) {
                USE_LOG_ERROR("LuaScripting: File not found: %s", filename.c_str());
                return false;
            }
        }

        if (luaL_dofile(m_L, resolved.c_str()) != LUA_OK) {
            USE_LOG_ERROR("LuaScripting: Error executing file: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool LuaScripting::DoString(const std::string& chunk) {
        if (!m_initialized) return false;
        if (luaL_dostring(m_L, chunk.c_str()) != LUA_OK) {
            USE_LOG_ERROR("LuaScripting: Error executing chunk: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    std::string LuaScripting::GetGlobalString(const std::string& name) {
        if (!m_initialized) return "";
        lua_getglobal(m_L, name.c_str());
        std::string result = lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return result;
    }

    bool LuaScripting::CallFunction(const std::string& funcName, float arg) {
        if (!m_initialized) return false;
        lua_getglobal(m_L, funcName.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 1);
            return false;
        }
        lua_pushnumber(m_L, arg);
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("LuaScripting: Error calling function '%s': %s",
                          funcName.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool LuaScripting::CallFunction(const std::string& funcName, const std::string& arg) {
        if (!m_initialized) return false;
        lua_getglobal(m_L, funcName.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 1);
            return false;
        }
        lua_pushstring(m_L, arg.c_str());
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("LuaScripting: Error calling function '%s': %s",
                          funcName.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    bool LuaScripting::CallFunction(const std::string& funcName) {
        if (!m_initialized) return false;
        lua_getglobal(m_L, funcName.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 1);
            return false;
        }
        if (lua_pcall(m_L, 0, 0, 0) != LUA_OK) {
            USE_LOG_ERROR("LuaScripting: Error calling function '%s': %s",
                          funcName.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

} // namespace USE