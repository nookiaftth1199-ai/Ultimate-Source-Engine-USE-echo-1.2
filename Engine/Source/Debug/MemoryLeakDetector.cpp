// ============================================================
// MemoryLeakDetector.cpp
// ============================================================
#include "MemoryLeakDetector.h"
#include "Logger.h"
#include <cstdlib>
#include <map>

namespace USE {
    static std::map<void*, MemoryLeakDetector::Allocation> s_allocs;

    MemoryLeakDetector& MemoryLeakDetector::Get() { static MemoryLeakDetector instance; return instance; }

    void MemoryLeakDetector::OnAlloc(void* ptr, size_t size, const char* file, int line) {
        if (!m_enabled) return;
        s_allocs[ptr] = {size, file, line};
    }

    void MemoryLeakDetector::OnFree(void* ptr) {
        if (!m_enabled) return;
        s_allocs.erase(ptr);
    }

    void MemoryLeakDetector::DumpLeaks() {
        if (s_allocs.empty()) {
            USE_LOG_INFO("MemoryLeakDetector: No leaks detected.");
            return;
        }
        USE_LOG_ERROR("MemoryLeakDetector: %zu leaks detected:", s_allocs.size());
        for (const auto& p : s_allocs) {
            USE_LOG_ERROR("  Leak at %p: %zu bytes, allocated at %s:%d",
                          p.first, p.second.size, p.second.file, p.second.line);
        }
    }
}