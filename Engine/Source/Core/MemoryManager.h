// ============================================================
// Ultimate Source Engine - Memory Manager
// ============================================================
//
// Provides custom memory allocation with tracking, debugging,
// and various allocator strategies (heap, pool, stack, etc.).
// ============================================================

#pragma once

#include "stdafx.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>
#include <vector>
#include <string>

namespace USE {

    // -----------------------------------------------------------------
    // Memory statistics structure
    // -----------------------------------------------------------------
    struct MemoryStats {
        size_t totalAllocated;      // Total bytes allocated (currently live)
        size_t totalFreed;          // Total bytes freed
        size_t allocationCount;     // Number of live allocations
        size_t peakAllocation;       // Peak memory usage (bytes)
        size_t failedAllocations;    // Number of failed allocation attempts

        MemoryStats()
            : totalAllocated(0)
            , totalFreed(0)
            , allocationCount(0)
            , peakAllocation(0)
            , failedAllocations(0)
        {}
    };

    // -----------------------------------------------------------------
    // Allocation type (for different memory pools)
    // -----------------------------------------------------------------
    enum class AllocationType {
        General,        // General purpose (malloc/free)
        Persistent,     // Persistent data (loaded once)
        Transient,      // Temporary per-frame data
        Texture,        // Texture resources
        Mesh,           // Mesh data
        Audio,          // Audio buffers
        Physics,        // Physics data
        Script,         // Scripting data
        COUNT
    };

    // -----------------------------------------------------------------
    // Allocation information for tracking
    // -----------------------------------------------------------------
    struct AllocationInfo {
        void*       ptr;            // Pointer to allocated memory
        size_t      size;           // Size in bytes
        AllocationType type;        // Type of allocation
        uint32_t    alignment;      // Alignment used
        const char* file;           // Source file (if tracking enabled)
        int         line;           // Source line
        uint64_t    timestamp;      // Allocation time
        uint32_t    threadId;       // Thread ID that allocated

        AllocationInfo()
            : ptr(nullptr)
            , size(0)
            , type(AllocationType::General)
            , alignment(0)
            , file(nullptr)
            , line(0)
            , timestamp(0)
            , threadId(0)
        {}
    };

    // -----------------------------------------------------------------
    // Memory pool interface (base class for custom allocators)
    // -----------------------------------------------------------------
    class MemoryPool {
    public:
        virtual ~MemoryPool() {}
        virtual void* Allocate(size_t size, size_t alignment) = 0;
        virtual void  Free(void* ptr) = 0;
        virtual size_t GetSize(void* ptr) const = 0;
        virtual void  Reset() = 0;
        virtual bool  Owns(void* ptr) const = 0;
        virtual const char* GetName() const = 0;
    };

    // -----------------------------------------------------------------
    // Main Memory Manager (accessed via Engine)
    // -----------------------------------------------------------------
    class MemoryManager {
    public:
        MemoryManager();
        ~MemoryManager();

        // Initialization / shutdown
        bool Initialize();
        void Shutdown();

        // Core allocation functions
        void* Allocate(size_t size, AllocationType type = AllocationType::General, 
                       size_t alignment = 16, const char* file = nullptr, int line = 0);
        void  Free(void* ptr);
        void* Reallocate(void* ptr, size_t newSize, AllocationType type = AllocationType::General,
                         size_t alignment = 16, const char* file = nullptr, int line = 0);

        // Get size of allocated block
        size_t GetSize(void* ptr) const;

        // Statistics
        const MemoryStats& GetStats() const { return m_stats; }
        void               ResetStats();

        // Enable/disable tracking (for performance)
        void SetTrackingEnabled(bool enabled) { m_trackingEnabled = enabled; }
        bool IsTrackingEnabled() const { return m_trackingEnabled; }

        // Dump allocation information (leak detection)
        void DumpAllocations(const char* filename = nullptr) const;
        size_t CheckForLeaks() const;  // Returns number of leaks

        // Pool management
        void RegisterPool(const char* name, MemoryPool* pool);
        void UnregisterPool(MemoryPool* pool);

        // Override global new/delete operators (optional, call to hook)
        static void HookGlobalOperators();
        static void UnhookGlobalOperators();

    private:
        // Allocation map (for tracking)
        typedef std::map<void*, AllocationInfo> AllocationMap;

        mutable std::mutex  m_mutex;           // Thread safety
        AllocationMap       m_allocations;      // Tracked allocations (if tracking on)
        MemoryStats         m_stats;
        bool                m_trackingEnabled;
        bool                m_initialized;

        // Pools (by type)
        MemoryPool*         m_pools[static_cast<int>(AllocationType::COUNT)];

        // Default pool (general heap)
        class HeapPool*     m_defaultPool;

        // Internal: find pool for allocation type
        MemoryPool* GetPool(AllocationType type) const;

        // Internal: allocation with tracking
        void* TrackAllocation(void* ptr, size_t size, AllocationType type, 
                              size_t alignment, const char* file, int line);
        void  TrackDeallocation(void* ptr);
    };

} // namespace USE