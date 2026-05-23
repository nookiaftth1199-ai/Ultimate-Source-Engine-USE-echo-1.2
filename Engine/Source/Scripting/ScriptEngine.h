// ============================================================
// Ultimate Source Engine - Script Engine
//============================================================
//
// Manages Lua/Python scripting. Provides methods to load scripts,
// call functions, and manage script objects bound to entities.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <memory>

namespace USE {

    // Forward declarations
    class Entity;

    // Opaque handle to a loaded script instance (bound to an entity)
    class ScriptObject {
    public:
        virtual ~ScriptObject() = default;
        // The actual implementation is backend‑specific (Lua, Python)
    };

    class ScriptEngine {
    public:
        static ScriptEngine& Get();

        // Initialize the scripting subsystem.
        bool Initialize();
        void Shutdown();

        // Load a script file and attach it to an entity.
        // Returns a ScriptObject handle (nullptr on failure).
        ScriptObject* LoadScript(const std::string& filename, Entity* owner);

        // Release a script object (call when entity is destroyed).
        void ReleaseScriptObject(ScriptObject* obj);

        // Call a function on a script object (no arguments).
        void CallFunction(ScriptObject* obj, const std::string& functionName);

        // Call a function with a single string argument.
        void CallFunction(ScriptObject* obj, const std::string& functionName, const std::string& arg);

        // Call a function with a float argument (for OnUpdate).
        void CallFunction(ScriptObject* obj, const std::string& functionName, float deltaTime);

        // Check if a function exists on a script object.
        bool HasFunction(ScriptObject* obj, const std::string& functionName) const;

    private:
        ScriptEngine() = default;
        ~ScriptEngine() = default;

        // Pimpl or backend‑specific data
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

} // namespace USE