// ============================================================
// Ultimate Source Engine - Console History Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleHistory.h"
#include "Utility/StringUtils.h"
#include <algorithm>

namespace USE {

    ConsoleHistory::ConsoleHistory(size_t maxSize)
        : m_maxSize(maxSize)
        , m_currentIndex(-1)
    {
    }

    void ConsoleHistory::Add(const std::string& command)
    {
        if (command.empty()) return;

        // Don't add duplicate of the last command
        if (!m_commands.empty() && m_commands.back() == command)
            return;

        m_commands.push_back(command);

        // Trim history to max size
        while (m_commands.size() > m_maxSize)
            m_commands.pop_front();

        // Reset navigation
        Reset();
    }

    std::string ConsoleHistory::GetPrevious()
    {
        if (m_commands.empty()) return "";

        if (m_currentIndex < 0) {
            // Start from the most recent
            m_currentIndex = static_cast<int>(m_commands.size()) - 1;
        } else if (m_currentIndex > 0) {
            --m_currentIndex;
        }
        return m_commands[m_currentIndex];
    }

    std::string ConsoleHistory::GetNext()
    {
        if (m_commands.empty() || m_currentIndex < 0) return "";

        ++m_currentIndex;
        if (m_currentIndex >= static_cast<int>(m_commands.size())) {
            Reset();
            return "";
        }
        return m_commands[m_currentIndex];
    }

    void ConsoleHistory::Reset()
    {
        m_currentIndex = -1;
    }

    void ConsoleHistory::Clear()
    {
        m_commands.clear();
        Reset();
    }

    std::vector<std::string> ConsoleHistory::FindMatches(const std::string& prefix) const
    {
        std::vector<std::string> matches;
        if (prefix.empty()) return matches;

        std::string lowerPrefix = StringUtils::ToLower(prefix);
        for (const auto& cmd : m_commands) {
            std::string lowerCmd = StringUtils::ToLower(cmd);
            if (lowerCmd.find(lowerPrefix) == 0) {
                matches.push_back(cmd);
            }
        }
        return matches;
    }

    void ConsoleHistory::SetMaxSize(size_t maxSize)
    {
        m_maxSize = maxSize;
        while (m_commands.size() > m_maxSize)
            m_commands.pop_front();
    }

} // namespace USE