// ============================================================
// Ultimate Source Engine - Finite State Machine Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "FiniteStateMachine.h"
#include "Core/Logger.h"

namespace USE {

    FiniteStateMachine::FiniteStateMachine()
        : m_currentState(nullptr)
    {
    }

    FiniteStateMachine::~FinishStateMachine()
    {
        // States are automatically deleted via unique_ptr
    }

    void FiniteStateMachine::AddState(const std::string& name, std::unique_ptr<State> state)
    {
        if (m_states.find(name) != m_states.end()) {
            USE_LOG_WARN("FSM: State '%s' already exists, overwriting.", name.c_str());
        }
        m_states[name] = std::move(state);
    }

    void FiniteStateMachine::SetInitialState(const std::string& name)
    {
        auto it = m_states.find(name);
        if (it == m_states.end()) {
            USE_LOG_ERROR("FSM: Initial state '%s' not found.", name.c_str());
            return;
        }
        if (m_currentState) {
            m_currentState->OnExit();
        }
        m_currentState = it->second.get();
        m_currentStateName = name;
        m_currentState->OnEnter();
    }

    void FiniteStateMachine::AddTransition(const std::string& fromState,
                                           const std::string& event,
                                           const std::string& toState)
    {
        m_transitions[fromState][event] = toState;
    }

    void FiniteStateMachine::Update(float deltaTime)
    {
        if (m_currentState) {
            m_currentState->OnUpdate(deltaTime);
        }
    }

    void FiniteStateMachine::TriggerEvent(const std::string& eventName)
    {
        if (!m_currentState) return;

        auto it = m_transitions.find(m_currentStateName);
        if (it == m_transitions.end()) return;

        auto eventIt = it->second.find(eventName);
        if (eventIt == it->second.end()) return;

        const std::string& targetStateName = eventIt->second;
        TransitionTo(targetStateName);
    }

    void FiniteStateMachine::TransitionTo(const std::string& stateName)
    {
        auto it = m_states.find(stateName);
        if (it == m_states.end()) {
            USE_LOG_ERROR("FSM: Cannot transition to unknown state '%s'.", stateName.c_str());
            return;
        }

        if (m_currentState) {
            m_currentState->OnExit();
        }
        m_currentState = it->second.get();
        m_currentStateName = stateName;
        m_currentState->OnEnter();
    }

} // namespace USE