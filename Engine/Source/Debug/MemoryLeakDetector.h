// ============================================================
// MemoryLeakDetector.h
// ============================================================
#pragma once
#include <cstddef>

namespace USE {
    class MemoryLeakDetector {
    public:
        static MemoryLeakDetector& Get();

        void Enable() { m_enabled = true; }
        void Disable() { m_enabled = false; }
        void OnAlloc(void* ptr, size_t size, const char* file, int line);
        void OnFree(void* ptr);
        void DumpLeaks();

    private:
        MemoryLeakDetector() = default;
        bool m_enabled = false;
        struct Allocation { size_t size; const char* file; int line; };
        void* m_allocations[1024]; // simple array for demo – use map in production
        size_t m_allocCount = 0;
    };
}