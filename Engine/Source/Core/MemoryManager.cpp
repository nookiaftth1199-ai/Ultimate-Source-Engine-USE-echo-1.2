// ============================================================
// Ultimate Source Engine - Memory Manager Implementation
// ============================================================

#include "stdafx.h"
#include "MemoryManager.h"
#include "Platform.h"
#include "Logger.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
    #include <malloc.h>  // for _aligned_malloc / _aligned_free
#endif

namespace USE {

    // -----------------------------------------------------------------
    // Simple heap pool (wraps malloc/free with alignment support)
    // -----------------------------------------------------------------
    class HeapPool : public MemoryPool {
    public:
        HeapPool(const char* name) : m_name(name) {}
        virtual ~HeapPool() {}

        void* Allocate(size_t size, size_t alignment) override {
            if (size == 0) return nullptr;

            void* ptr = nullptr;
#ifdef _WIN32
            ptr = _aligned_malloc(size, alignment);
#else
            if (alignment <= sizeof(void*)) {
                ptr = malloc(size);
            } else {
                // posix_memalign expects alignment to be multiple of sizeof(void*) and power of two
                if (alignment < sizeof(void*)) alignment = sizeof(void*);
                // ensure alignment is power of two
                if (alignment & (alignment - 1)) {
                    // round up to next power of two
                    size_t power = 1;
                    while (power < alignment) power <<= 1;
                    alignment = power;
                }
                posix_memalign(&ptr, alignment, size);
            }
#endif
            return ptr;
        }

        void Free(void* ptr) override {
            if (!ptr) return;
#ifdef _WIN32
            _aligned_free(ptr);
#else
            free(ptr);
#endif
        }

        size_t GetSize(void* ptr) const override {
            // Not easily portable; we could store size separately.
            // For now, return 0 (tracking will handle size)
            return 0;
        }

        void Reset() override {
            // Heap pool cannot reset without freeing everything; we ignore.
        }

        bool Owns(void* ptr) const override {
            // Heap pool owns everything not owned by other pools? Actually, it's the default,
            // but we cannot determine ownership without extra tracking.
            return true; // fallback
        }

        const char* GetName() const override { return m_name.c_str(); }

    private:
        std::string m_name;
    };

    // -----------------------------------------------------------------
    // MemoryManager implementation
    // -----------------------------------------------------------------

    MemoryManager::MemoryManager()
        : m_trackingEnabled(true)   // Enable tracking by default in debug builds; could be controlled by macro
        , m_initialized(false)
        , m_defaultPool(nullptr)
    {
        for (int i = 0; i < static_cast<int>(AllocationType::COUNT); ++i) {
            m_pools[i] = nullptr;
        }
    }

    MemoryManager::~MemoryManager()
    {
        Shutdown();
    }

    bool MemoryManager::Initialize()
    {
        if (m_initialized) return true;

        USE_LOG_INFO("Initializing MemoryManager...");

        // Create default heap pool
        m_defaultPool = new HeapPool("DefaultHeap");

        // Initialize pools for each allocation type (can be customized later)
        for (int i = 0; i < static_cast<int>(AllocationType::COUNT); ++i) {
            m_pools[i] = m_defaultPool; // All use default heap initially
        }

        m_initialized = true;
        USE_LOG_INFO("MemoryManager initialized.");
        return true;
    }

    void MemoryManager::Shutdown()
    {
        if (!m_initialized) return;

        USE_LOG_INFO("Shutting down MemoryManager...");

        // Check for leaks if tracking is enabled
        if (m_trackingEnabled) {
            size_t leaks = CheckForLeaks();
            if (leaks > 0) {
                USE_LOG_ERROR("Memory leaks detected: %zu allocations not freed!", leaks);
                DumpAllocations("leaks.txt");
            } else {
                USE_LOG_INFO("No memory leaks detected.");
            }
        }

        // Delete pools (except default pool, which will be deleted last)
        for (int i = 0; i < static_cast<int>(AllocationType::COUNT); ++i) {
            if (m_pools[i] != m_defaultPool) {
                delete m_pools[i];
                m_pools[i] = nullptr;
            }
        }

        if (m_defaultPool) {
            delete m_defaultPool;
            m_defaultPool = nullptr;
        }

        m_allocations.clear();
        m_initialized = false;

        USE_LOG_INFO("MemoryManager shutdown complete.");
    }

    void* MemoryManager::Allocate(size_t size, AllocationType type, size_t alignment,
                                   const char* file, int line)
    {
        if (size == 0) return nullptr;

        MemoryPool* pool = GetPool(type);
        if (!pool) {
            USE_LOG_ERROR("No memory pool for allocation type %d", static_cast<int>(type));
            return nullptr;
        }

        void* ptr = pool->Allocate(size, alignment);
        if (!ptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stats.failedAllocations++;
            USE_LOG_ERROR("Allocation failed: %zu bytes, type %d", size, static_cast<int>(type));
            return nullptr;
        }

        if (m_trackingEnabled) {
            ptr = TrackAllocation(ptr, size, type, alignment, file, line);
        }

        return ptr;
    }

    void MemoryManager::Free(void* ptr)
    {
        if (!ptr) return;

        if (m_trackingEnabled) {
            TrackDeallocation(ptr);
        }

        // Find which pool owns this pointer
        // Since we don't have reverse mapping, we'll try all pools.
        // Optimization: we could store pool id in allocation info.
        bool freed = false;
        for (int i = 0; i < static_cast<int>(AllocationType::COUNT); ++i) {
            MemoryPool* pool = m_pools[i];
            if (pool && pool->Owns(ptr)) {
                pool->Free(ptr);
                freed = true;
                break;
            }
        }
        if (!freed) {
            // Fallback to default pool
            m_defaultPool->Free(ptr);
        }
    }

    void* MemoryManager::Reallocate(void* ptr, size_t newSize, AllocationType type,
                                     size_t alignment, const char* file, int line)
    {
        if (!ptr) {
            return Allocate(newSize, type, alignment, file, line);
        }
        if (newSize == 0) {
            Free(ptr);
            return nullptr;
        }

        // Find pool
        MemoryPool* pool = GetPool(type);
        if (!pool) {
            USE_LOG_ERROR("No memory pool for reallocation type %d", static_cast<int>(type));
            return nullptr;
        }

        // For simplicity, we allocate new, copy, free old.
        void* newPtr = Allocate(newSize, type, alignment, file, line);
        if (newPtr) {
            size_t oldSize = GetSize(ptr);
            if (oldSize > 0) {
                memcpy(newPtr, ptr, std::min(oldSize, newSize));
            }
            Free(ptr);
        }
        return newPtr;
    }

    size_t MemoryManager::GetSize(void* ptr) const
    {
        if (!ptr) return 0;

        if (m_trackingEnabled) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_allocations.find(ptr);
            if (it != m_allocations.end()) {
                return it->second.size;
            }
        }

        // Fallback: try pools
        for (int i = 0; i < static_cast<int>(AllocationType::COUNT); ++i) {
            MemoryPool* pool = m_pools[i];
            if (pool) {
                size_t sz = pool->GetSize(ptr);
                if (sz > 0) return sz;
            }
        }
        return 0;
    }

    void MemoryManager::ResetStats()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stats = MemoryStats();
    }

    void MemoryManager::DumpAllocations(const char* filename) const
    {
        if (!m_trackingEnabled || m_allocations.empty()) return;

        FILE* file = stdout;
        if (filename) {
#ifdef _WIN32
            fopen_s(&file, filename, "w");
#else
            file = fopen(filename, "w");
#endif
            if (!file) file = stdout;
        }

        fprintf(file, "=== Memory Allocation Dump ===\n");
        fprintf(file, "Total allocations: %zu\n", m_allocations.size());
        fprintf(file, "Total memory: %zu bytes\n", m_stats.totalAllocated);

        for (const auto& pair : m_allocations) {
            const AllocationInfo& info = pair.second;
            fprintf(file, "Ptr: %p, Size: %zu, Type: %d, Align: %zu, File: %s:%d\n",
                    info.ptr, info.size, static_cast<int>(info.type),
                    info.alignment, info.file ? info.file : "unknown", info.line);
        }

        fprintf(file, "================================\n");

        if (file != stdout) fclose(file);
    }

    size_t MemoryManager::CheckForLeaks() const
    {
        if (!m_trackingEnabled) return 0;
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_allocations.size();
    }

    void MemoryManager::RegisterPool(const char* name, MemoryPool* pool)
    {
        // For simplicity, we don't map pool to type automatically; user must assign.
        // Could extend to have pool per type.
        USE_LOG_INFO("Pool registered: %s", name);
    }

    void MemoryManager::UnregisterPool(MemoryPool* pool)
    {
        // Not implemented for now.
    }

    MemoryPool* MemoryManager::GetPool(AllocationType type) const
    {
        int idx = static_cast<int>(type);
        if (idx >= 0 && idx < static_cast<int>(AllocationType::COUNT)) {
            return m_pools[idx];
        }
        return m_defaultPool;
    }

    void* MemoryManager::TrackAllocation(void* ptr, size_t size, AllocationType type,
                                          size_t alignment, const char* file, int line)
    {
        if (!ptr) return nullptr;

        std::lock_guard<std::mutex> lock(m_mutex);

        AllocationInfo info;
        info.ptr = ptr;
        info.size = size;
        info.type = type;
        info.alignment = alignment;
        info.file = file;
        info.line = line;
        info.timestamp = Platform::GetMicroseconds();
        info.threadId = Platform::GetThreadID();

        m_allocations[ptr] = info;

        m_stats.totalAllocated += size;
        m_stats.allocationCount++;
        if (m_stats.totalAllocated > m_stats.peakAllocation) {
            m_stats.peakAllocation = m_stats.totalAllocated;
        }

        return ptr;
    }

    void MemoryManager::TrackDeallocation(void* ptr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_allocations.find(ptr);
        if (it != m_allocations.end()) {
            m_stats.totalFreed += it->second.size;
            m_stats.totalAllocated -= it->second.size;
            m_stats.allocationCount--;
            m_allocations.erase(it);
        }
    }

    // -----------------------------------------------------------------
    // Global operator overrides (optional)
    // -----------------------------------------------------------------
    // These replace global new/delete with our memory manager.
    // To enable, call MemoryManager::HookGlobalOperators() after initialization.

    static MemoryManager* g_GlobalMemoryManager = nullptr;

    void MemoryManager::HookGlobalOperators()
    {
        g_GlobalMemoryManager = Engine::Get()->GetMemoryManager(); // Need Engine access
    }

    void MemoryManager::UnhookGlobalOperators()
    {
        g_GlobalMemoryManager = nullptr;
    }

} // namespace USE

// -----------------------------------------------------------------
// Global new/delete operators
// -----------------------------------------------------------------
void* operator new(size_t size)
{
    if (USE::g_GlobalMemoryManager) {
        return USE::g_GlobalMemoryManager->Allocate(size, USE::AllocationType::General, 16, __FILE__, __LINE__);
    }
    return malloc(size);
}

void* operator new[](size_t size)
{
    if (USE::g_GlobalMemoryManager) {
        return USE::g_GlobalMemoryManager->Allocate(size, USE::AllocationType::General, 16, __FILE__, __LINE__);
    }
    return malloc(size);
}

void operator delete(void* ptr) noexcept
{
    if (USE::g_GlobalMemoryManager) {
        USE::g_GlobalMemoryManager->Free(ptr);
    } else {
        free(ptr);
    }
}

void operator delete[](void* ptr) noexcept
{
    if (USE::g_GlobalMemoryManager) {
        USE::g_GlobalMemoryManager->Free(ptr);
    } else {
        free(ptr);
    }
}