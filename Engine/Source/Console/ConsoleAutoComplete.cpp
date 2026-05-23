// ============================================================
// Ultimate Source Engine - Console Auto‑Completion Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleAutoComplete.h"
#include "Utility/StringUtils.h"

namespace USE {

    ConsoleAutoComplete::ConsoleAutoComplete()
        : m_currentIndex(-1)
    {
    }

    void ConsoleAutoComplete::Update(const std::string& input, const std::vector<std::string>& candidates)
    {
        m_baseInput = input;
        m_suggestions.clear();

        if (input.empty()) {
            m_currentIndex = -1;
            return;
        }

        std::string lowerInput = StringUtils::ToLower(input);

        for (const auto& candidate : candidates) {
            std::string lowerCandidate = StringUtils::ToLower(candidate);
            if (lowerCandidate.find(lowerInput) == 0) {
                m_suggestions.push_back(candidate);
            }
        }

        // Sort alphabetically
        std::sort(m_suggestions.begin(), m_suggestions.end());

        if (!m_suggestions.empty()) {
            m_currentIndex = 0;
        } else {
            m_currentIndex = -1;
        }
    }

    std::string ConsoleAutoComplete::GetNext()
    {
        if (m_suggestions.empty()) return m_baseInput;

        m_currentIndex = (m_currentIndex + 1) % static_cast<int>(m_suggestions.size());
        return m_suggestions[m_currentIndex];
    }

    std::string ConsoleAutoComplete::GetPrevious()
    {
        if (m_suggestions.empty()) return m_baseInput;

        m_currentIndex--;
        if (m_currentIndex < 0) {
            m_currentIndex = static_cast<int>(m_suggestions.size()) - 1;
        }
        return m_suggestions[m_currentIndex];
    }

    std::string ConsoleAutoComplete::GetCurrent() const
    {
        if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_suggestions.size())) {
            return m_suggestions[m_currentIndex];
        }
        return m_baseInput;
    }

    void ConsoleAutoComplete::Reset()
    {
        m_suggestions.clear();
        m_baseInput.clear();
        m_currentIndex = -1;
    }

} // namespace USE