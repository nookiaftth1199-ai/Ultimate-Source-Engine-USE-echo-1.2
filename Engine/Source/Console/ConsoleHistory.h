// ============================================================
// Ultimate Source Engine - Console History
// ============================================================
//
// Manages the command history for the developer console.
// Provides navigation (up/down) and prefix-based search.
// ============================================================

#pragma once

#include "stdafx.h"
#include <deque>
#include <string>
#include <vector>

namespace USE {

    class ConsoleHistory {
    public:
        ConsoleHistory(size_t maxSize = 100);
        ~ConsoleHistory() = default;

        // Add a command to history (skips duplicates with previous)
        void Add(const std::string& command);

        // Get previous command (move backward in history)
        std::string GetPrevious();

        // Get next command (move forward in history)
        std::string GetNext();

        // Reset navigation to the newest entry
        void Reset();

        // Clear all history
        void Clear();

        // Find commands that start with a given prefix
        std::vector<std::string> FindMatches(const std::string& prefix) const;

        // Set maximum history size
        void SetMaxSize(size_t maxSize);

        // Get current size
        size_t GetSize() const { return m_commands.size(); }

        // Get all commands (for saving)
        const std::deque<std::string>& GetCommands() const { return m_commands; }

    private:
        std::deque<std::string> m_commands;
        size_t                  m_maxSize;
        int                     m_currentIndex; // -1 when not browsing
    };

} // namespace USE