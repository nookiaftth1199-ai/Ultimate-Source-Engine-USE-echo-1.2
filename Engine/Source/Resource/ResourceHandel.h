// ============================================================
// Ultimate Source Engine - Resource Handle
// ============================================================
//
// A lightweight non‑owning handle to a resource.
// Provides safe access and validity check (if needed).
// ============================================================

#pragma once

namespace USE {

    template<typename T>
    class ResourceHandle {
    public:
        ResourceHandle() : m_resource(nullptr) {}
        explicit ResourceHandle(T* resource) : m_resource(resource) {}

        T* operator->() const { return m_resource; }
        T& operator*() const { return *m_resource; }
        explicit operator bool() const { return m_resource != nullptr; }
        T* Get() const { return m_resource; }

        bool IsValid() const { return m_resource != nullptr; }

    private:
        T* m_resource;
    };

} // namespace USE