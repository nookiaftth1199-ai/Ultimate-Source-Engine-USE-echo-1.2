// ============================================================
// Ultimate Source Engine - Script Component
// ============================================================
//
// Component that attaches a script (Lua/Python) to an entity.
// The script can define functions like OnCreate, OnUpdate, OnDestroy.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Entity/Component.h"
#include <string>

namespace USE {

    // Forward declaration
    class ScriptObject;

    class ScriptComponent : public Component {
    public:
        ScriptComponent();
        virtual ~ScriptComponent();

        // Load a script from the given path.
        // Returns true on success.
        bool LoadScript(const std::string& path);

        // Call a function on the script (with optional argument string).
        void CallFunction(const std::string& function);
        void CallFunction(const std::string& function, const std::string& arg);

        // Check if the component has a valid script loaded.
        bool HasScript() const { return m_scriptObject != nullptr; }

        // Get the script path.
        const std::string& GetScriptPath() const { return m_scriptPath; }

        // Overrides from Component
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update(float deltaTime) override;

        virtual const char* GetTypeName() const override { return "ScriptComponent"; }

    private:
        std::string     m_scriptPath;
        ScriptObject*   m_scriptObject;   // owned by the scripting system
    };

} // namespace USE