// ============================================================
// Ultimate Source Engine - Finite State Machine
//============================================================
//
// Generic finite state machine with state classes, transitions
// triggered by events, and per‑state update.
// ============================================================

#pragma once

#include "stdafx.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace USE {

    // Base class for all states
    class State {
    public:
        virtual ~State() = default;

        // Called when entering the state
        virtual void OnEnter() {}

        // Called every frame while in the state
        virtual void OnUpdate(float deltaTime) {}

        // Called when leaving the state
        virtual void OnExit() {}

        // Get the state name (for debugging)
        virtual const char* GetName() const = 0;
    };

    class FiniteStateMachine {
    public:
        FiniteStateMachine();
        ~FiniteStateMachine();

        // Add a state (takes ownership)
        void AddState(const std::string& name, std::unique_ptr<State> state);

        // Set the initial state (must be added first)
        void SetInitialState(const std::string& name);

        // Add a transition: fromState + event -> toState
        void AddTransition(const std::string& fromState,
                           const std::string& event,
                           const std::string& toState);

        // Update the current state (calls its OnUpdate)
        void Update(float deltaTime);

        // Trigger an event; if a transition exists, change state
        void TriggerEvent(const std::string& eventName);

        // Directly transition to a state (bypassing events)
        void TransitionTo(const std::string& stateName);

        // Get current state name (or empty string if none)
        const std::string& GetCurrentStateName() const { return m_currentStateName; }

        // Get current state pointer (for external queries)
        State* GetCurrentState() const { return m_currentState; }

    private:
        std::unordered_map<std::string, std::unique_ptr<State>> m_states;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_transitions; // fromState -> (event -> toState)
        State* m_currentState;
        std::string m_currentStateName;
    };

} // namespace USE