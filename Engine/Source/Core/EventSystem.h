// ============================================================
// Ultimate Source Engine - Event System
// ============================================================
//
// Provides a type-safe, centralized event dispatch system.
// Any type can be used as an event (plain struct, class, etc.).
// Subscribers receive callbacks when events of that type are published.
// Returns a token for unsubscription.
// ============================================================

#pragma once

#include <functional>
#include <vector>
#include <map>
#include <cstdint>

namespace USE {

    // -----------------------------------------------------------------
    // Optional base class for events (not required)
    // -----------------------------------------------------------------
    class Event {
    public:
        virtual ~Event() {}
    };

    // -----------------------------------------------------------------
    // EventManager - central hub for event subscription and dispatch
    // -----------------------------------------------------------------
    class EventManager {
    public:
        // Subscribe to an event type. Returns a token used to unsubscribe.
        template<typename EventType>
        uint64_t Subscribe(std::function<void(const EventType&)> callback) {
            return GetDispatcher<EventType>().Subscribe(callback);
        }

        // Unsubscribe from an event type using the token returned by Subscribe.
        template<typename EventType>
        void Unsubscribe(uint64_t token) {
            GetDispatcher<EventType>().Unsubscribe(token);
        }

        // Publish an event to all subscribers.
        template<typename EventType>
        void Publish(const EventType& event) {
            GetDispatcher<EventType>().Publish(event);
        }

        // Remove all subscribers for a specific event type.
        template<typename EventType>
        void ClearSubscriptions() {
            GetDispatcher<EventType>().Clear();
        }

    private:
        template<typename EventType>
        class Dispatcher {
        public:
            using Callback = std::function<void(const EventType&)>;

            uint64_t Subscribe(Callback cb) {
                uint64_t token = ++m_nextToken;
                m_callbacks.push_back(cb);
                m_tokenMap[token] = m_callbacks.size() - 1;
                return token;
            }

            void Unsubscribe(uint64_t token) {
                auto it = m_tokenMap.find(token);
                if (it != m_tokenMap.end()) {
                    size_t index = it->second;
                    // Remove by swapping with last element (O(1))
                    m_callbacks[index] = m_callbacks.back();
                    m_callbacks.pop_back();

                    // Update token map for the element that moved
                    for (auto& pair : m_tokenMap) {
                        if (pair.second == m_callbacks.size()) {
                            pair.second = index;
                            break;
                        }
                    }
                    m_tokenMap.erase(it);
                }
            }

            void Publish(const EventType& event) {
                // Simple iteration. If a callback unsubscribes, the vector may be modified.
                // For safety, we could iterate over a copy, but that is slower.
                // We assume callbacks are well-behaved (unsubscribe rarely during dispatch).
                for (auto& cb : m_callbacks) {
                    cb(event);
                }
            }

            void Clear() {
                m_callbacks.clear();
                m_tokenMap.clear();
                m_nextToken = 0;
            }

        private:
            std::vector<Callback> m_callbacks;
            std::map<uint64_t, size_t> m_tokenMap;
            uint64_t m_nextToken = 0;
        };

        template<typename EventType>
        static Dispatcher<EventType>& GetDispatcher() {
            static Dispatcher<EventType> dispatcher;
            return dispatcher;
        }
    };

} // namespace USE