// ============================================================
// Ultimate Source Engine - Console Log
// ============================================================
//
// Stores and manages console messages with filtering capabilities.
// ============================================================

#pragma once

#include "stdafx.h"
#include "ConsoleSystem.h" // for ConsoleMessage, ConsoleMessageType
#include <deque>
#include <bitset>
#include <string>

namespace USE {

    class ConsoleLog {
    public:
        ConsoleLog(size_t maxMessages = 1000);
        ~ConsoleLog() = default;

        // Add a message to the log (collapses consecutive duplicates)
        void AddMessage(const ConsoleMessage& msg);

        // Clear all messages
        void Clear();

        // Get all messages (raw)
        const std::deque<ConsoleMessage>& GetMessages() const { return m_messages; }

        // Get messages filtered by current filter settings
        std::deque<ConsoleMessage> GetFilteredMessages() const;

        // Filter by message type
        void SetMessageFilter(ConsoleMessageType type, bool enabled);

        // Filter by substring (case‑insensitive)
        void SetSearchFilter(const std::string& filter);
        void ClearSearchFilter();

        // Set maximum number of messages to keep
        void SetMaxMessages(size_t max);
        size_t GetMaxMessages() const { return m_maxMessages; }
        size_t GetMessageCount() const { return m_messages.size(); }

    private:
        std::deque<ConsoleMessage> m_messages;
        std::bitset<static_cast<int>(ConsoleMessageType::Count)> m_messageFilter;
        std::string m_searchFilter;
        size_t m_maxMessages;
    };

} // namespace USE