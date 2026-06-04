// ============================================================
// Ultimate Source Engine - Console Auto‑Completion
// ============================================================
//
// Handles tab‑completion for the developer console.
// Stores a list of suggestions for the current input prefix.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <string>

namespace USE {

    class ConsoleAutoComplete {
    public:
        ConsoleAutoComplete();
        ~ConsoleAutoComplete() = default;

        // Update suggestions based on current input and available candidates.
        void Update(const std::string& input, const std::vector<std::string>& candidates);

        // Get the next suggestion (cycle forward).
        std::string GetNext();

        // Get the previous suggestion (cycle backward).
        std::string GetPrevious();

        // Get the currently selected suggestion.
        std::string GetCurrent() const;

        // Check if there are any suggestions.
        bool HasSuggestions() const { return !m_suggestions.empty(); }

        // Get the list of suggestions.
        const std::vector<std::string>& GetSuggestions() const { return m_suggestions; }

        // Get the current index.
        int GetCurrentIndex() const { return m_currentIndex; }

        // Reset to empty state.
        void Reset();

    private:
        std::vector<std::string> m_suggestions;
        std::string              m_baseInput;
        int                      m_currentIndex;
    };

} // namespace USE