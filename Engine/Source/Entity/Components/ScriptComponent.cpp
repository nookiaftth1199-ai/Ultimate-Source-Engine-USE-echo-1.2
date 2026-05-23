// ============================================================
// Ultimate Source Engine - Script Component Implementation
// ============================================================

#include "stdafx.h"
#include "ScriptComponent.h"
#include "Core/Engine.h"
#include "Scripting/ScriptEngine.h"
#include "Core/Logger.h"

namespace USE {

    ScriptComponent::ScriptComponent()
        : m_scriptObject(nullptr)
    {
    }

    ScriptComponent::~ScriptComponent()
    {
        // The script object is owned by the script engine; we don't delete it here.
        // But we should inform the engine that this entity is no longer using it.
        if (m_scriptObject) {
            auto* scriptEngine = Engine::Get()->GetScriptEngine();
            if (scriptEngine) {
                scriptEngine->ReleaseScriptObject(m_scriptObject);
            }
        }
    }

    bool ScriptComponent::LoadScript(const std::string& path)
    {
        auto* scriptEngine = Engine::Get()->GetScriptEngine();
        if (!scriptEngine) {
            USE_LOG_ERROR("ScriptComponent: ScriptEngine not available");
            return false;
        }

        // Release previous script if any
        if (m_scriptObject) {
            scriptEngine->ReleaseScriptObject(m_scriptObject);
            m_scriptObject = nullptr;
        }

        m_scriptObject = scriptEngine->LoadScript(path, GetOwner());
        if (!m_scriptObject) {
            USE_LOG_ERROR("ScriptComponent: Failed to load script: %s", path.c_str());
            return false;
        }

        m_scriptPath = path;

        // Call OnCreate on the script if it exists
        CallFunction("OnCreate");

        return true;
    }

    void ScriptComponent::CallFunction(const std::string& function)
    {
        if (!m_scriptObject) return;
        auto* scriptEngine = Engine::Get()->GetScriptEngine();
        if (scriptEngine) {
            scriptEngine->CallFunction(m_scriptObject, function);
        }
    }

    void ScriptComponent::CallFunction(const std::string& function, const std::string& arg)
    {
        if (!m_scriptObject) return;
        auto* scriptEngine = Engine::Get()->GetScriptEngine();
        if (scriptEngine) {
            scriptEngine->CallFunction(m_scriptObject, function, arg);
        }
    }

    void ScriptComponent::OnAttach()
    {
        // Called when component is attached to an entity.
        // Could call a script function if needed, but script might not be loaded yet.
    }

    void ScriptComponent::OnDetach()
    {
        if (m_scriptObject) {
            CallFunction("OnDestroy");
            auto* scriptEngine = Engine::Get()->GetScriptEngine();
            if (scriptEngine) {
                scriptEngine->ReleaseScriptObject(m_scriptObject);
            }
            m_scriptObject = nullptr;
        }
    }

    void ScriptComponent::Update(float deltaTime)
    {
        if (m_scriptObject) {
            // Pass delta time as argument (convert to string for simplicity)
            // In a real engine, you'd have a more sophisticated parameter passing.
            std::string dt = std::to_string(deltaTime);
            CallFunction("OnUpdate", dt);
        }
    }

} // namespace USE