// ============================================================
// Ultimate Source Engine - Singleton Base Class
// ============================================================
//
// Provides a thread-safe base class for creating singletons.
// Derive from Singleton<T> to make T a singleton.
//
// Example:
//   class MyManager : public Singleton<MyManager> {
//       friend class Singleton<MyManager>;
//       MyManager() = default; // private constructor
//   public:
//       void DoSomething();
//   };
//
//   // Usage:
//   MyManager::Get()->DoSomething();
// ============================================================

#pragma once

#include <mutex>
#include <cstdlib>

namespace USE {

    // -----------------------------------------------------------------
    // Singleton template
    // -----------------------------------------------------------------
    template<typename T>
    class Singleton {
    public:
        // Get the singleton instance (creates it on first call)
        static T* Get() {
            std::call_once(m_onceFlag, []() {
                CreateInstance();
                std::atexit(DestroyInstance);
            });
            return m_instance;
        }

        // Check if the instance already exists without creating it
        static bool HasInstance() {
            return m_instance != nullptr;
        }

        // Explicitly destroy the instance (normally called automatically at exit)
        static void DestroyInstance() {
            if (m_instance) {
                delete m_instance;
                m_instance = nullptr;
            }
        }

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;

        // Disable copy and move
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;

    private:
        static void CreateInstance() {
            m_instance = new T();
        }

        static T* m_instance;
        static std::once_flag m_onceFlag;
    };

    // Static member definitions
    template<typename T>
    T* Singleton<T>::m_instance = nullptr;

    template<typename T>
    std::once_flag Singleton<T>::m_onceFlag;

} // namespace USE

// -----------------------------------------------------------------
// Convenience macro to declare friendship (optional)
// -----------------------------------------------------------------
#define USE_DECLARE_SINGLETON(Class) \
    friend class Singleton<Class>;