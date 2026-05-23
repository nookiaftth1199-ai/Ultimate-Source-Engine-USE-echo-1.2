// ============================================================
// Ultimate Source Engine - Console Log Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleLog.h"
#include "Utility/StringUtils.h"

namespace USE {

    ConsoleLog::ConsoleLog(size_t maxMessages)
        : m_maxMessages(maxMessages)
    {
        // Enable all message types by default
        m_messageFilter.set();
    }

    void ConsoleLog::AddMessage(const ConsoleMessage& msg)
    {
        // Collapse consecutive identical messages (excluding timestamp)
        if (!m_messages.empty()) {
            auto& last = m_messages.back();
            if (last.text == msg.text && last.type == msg.type && last.module == msg.module) {
                last.repeatCount++;
                return;
            }
        }

        m_messages.push_back(msg);

        // Trim to max size
        while (m_messages.size() > m_maxMessages) {
            m_messages.pop_front();
        }
    }

    void ConsoleLog::Clear()
    {
        m_messages.clear();
    }

    std::deque<ConsoleMessage> ConsoleLog::GetFilteredMessages() const
    {
        std::deque<ConsoleMessage> filtered;

        for (const auto& msg : m_messages) {
            // Check type filter
            if (!m_messageFilter.test(static_cast<int>(msg.type))) {
                continue;
            }

            // Check search filter
            if (!m_searchFilter.empty()) {
                std::string lowerText = StringUtils::ToLower(msg.text);
                if (lowerText.find(m_searchFilter) == std::string::npos) {
                    continue;
                }
            }

            filtered.push_back(msg);
        }

        return filtered;
    }

    void ConsoleLog::SetMessageFilter(ConsoleMessageType type, bool enabled)
    {
        m_messageFilter.set(static_cast<int>(type), enabled);
    }

    void ConsoleLog::SetSearchFilter(const std::string& filter)
    {
        m_searchFilter = StringUtils::ToLower(filter);
    }

    void ConsoleLog::ClearSearchFilter()
    {
        m_searchFilter.clear();
    }

    void ConsoleLog::SetMaxMessages(size_t max)
    {
        m_maxMessages = max;
        while (m_messages.size() > m_maxMessages) {
            m_messages.pop_front();
        }
    }

} // namespace USE