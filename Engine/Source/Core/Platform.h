// ============================================================
// Ultimate Source Engine - Platform Abstraction Layer
// ============================================================
//
// This header provides platform detection macros, basic types,
// and low-level OS utility functions.
//
// Supported platforms:
//   - Windows (32/64-bit)
//   - Linux (32/64-bit)
//   - macOS (Intel/Apple Silicon)
// ============================================================

#pragma once

#include "stdafx.h"

// -----------------------------------------------------------------
// Platform Detection
// -----------------------------------------------------------------

// Detect operating system
#if defined(_WIN32) || defined(_WIN64)
    #define USE_PLATFORM_WINDOWS 1
    #define USE_PLATFORM_LINUX   0
    #define USE_PLATFORM_MACOS   0
    #define USE_PLATFORM_NAME "Windows"
#elif defined(__linux__)
    #define USE_PLATFORM_WINDOWS 0
    #define USE_PLATFORM_LINUX   1
    #define USE_PLATFORM_MACOS   0
    #define USE_PLATFORM_NAME "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define USE_PLATFORM_WINDOWS 0
        #define USE_PLATFORM_LINUX   0
        #define USE_PLATFORM_MACOS   1
        #define USE_PLATFORM_NAME "macOS"
    #endif
#else
    #error "Unsupported platform!"
#endif

// Detect architecture (32-bit vs 64-bit)
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
    #define USE_ARCH_64BIT 1
    #define USE_ARCH_32BIT 0
    #define USE_ARCH_NAME "64-bit"
#else
    #define USE_ARCH_64BIT 0
    #define USE_ARCH_32BIT 1
    #define USE_ARCH_NAME "32-bit"
#endif

// Detect compiler
#if defined(_MSC_VER)
    #define USE_COMPILER_MSVC 1
    #define USE_COMPILER_GCC  0
    #define USE_COMPILER_CLANG 0
    #define USE_COMPILER_NAME "MSVC"
#elif defined(__clang__)
    #define USE_COMPILER_MSVC 0
    #define USE_COMPILER_GCC  0
    #define USE_COMPILER_CLANG 1
    #define USE_COMPILER_NAME "Clang"
#elif defined(__GNUC__)
    #define USE_COMPILER_MSVC 0
    #define USE_COMPILER_GCC  1
    #define USE_COMPILER_CLANG 0
    #define USE_COMPILER_NAME "GCC"
#else
    #define USE_COMPILER_MSVC 0
    #define USE_COMPILER_GCC  0
    #define USE_COMPILER_CLANG 0
    #define USE_COMPILER_NAME "Unknown"
#endif

// -----------------------------------------------------------------
// Compiler-specific Attributes
// -----------------------------------------------------------------

// Inlining
#define USE_FORCEINLINE __forceinline
#define USE_NEVER_INLINE __declspec(noinline)

// Alignment
#define USE_ALIGN(x) __declspec(align(x))

// Packing
#define USE_PACK_BEGIN __pragma(pack(push, 1))
#define USE_PACK_END   __pragma(pack(pop))

// Deprecation
#define USE_DEPRECATED __declspec(deprecated)

// -----------------------------------------------------------------
// Basic Types (ensure they match across platforms)
// -----------------------------------------------------------------

#include <stdint.h>
#include <stddef.h>

namespace USE {

    // Integer types (fixed width)
    typedef int8_t      int8;
    typedef uint8_t     uint8;
    typedef int16_t     int16;
    typedef uint16_t    uint16;
    typedef int32_t     int32;
    typedef uint32_t    uint32;
    typedef int64_t     int64;
    typedef uint64_t    uint64;

    // Floating point
    typedef float       float32;
    typedef double      float64;

    // Character types
    typedef char        ansichar;
    typedef wchar_t     widechar;

    // Boolean
    typedef bool        boolean;

    // Platform-specific handle
    #if USE_PLATFORM_WINDOWS
        typedef void*       Handle;
    #else
        typedef int         Handle;
    #endif

} // namespace USE

// -----------------------------------------------------------------
// Platform Utility Functions
// -----------------------------------------------------------------

namespace USE {
namespace Platform {

    // System information
    const char* GetPlatformName();          // Returns "Windows", "Linux", etc.
    const char* GetArchitectureName();       // Returns "32-bit" or "64-bit"
    const char* GetCompilerName();           // Returns "MSVC", "GCC", etc.
    
    // Get number of logical CPU cores
    int GetCPUCount();

    // Get current process ID
    uint32 GetProcessID();

    // Get current thread ID
    uint32 GetThreadID();

    // Get executable path (full path to the .exe)
    std::string GetExecutablePath();

    // Get engine working directory
    std::string GetWorkingDirectory();

    // Set current working directory
    bool SetWorkingDirectory(const std::string& path);

    // Environment variables
    std::string GetEnvironmentVariable(const std::string& name);
    bool SetEnvironmentVariable(const std::string& name, const std::string& value);

    // High-performance timer (microseconds)
    uint64 GetMicroseconds();

    // Memory page size (for allocators)
    size_t GetPageSize();

    // Dynamic library loading
    Handle LoadLibrary(const std::string& path);
    void*  GetLibrarySymbol(Handle lib, const std::string& symbol);
    void   UnloadLibrary(Handle lib);

    // Error handling (get last error string)
    std::string GetLastErrorString();

    // Debug output (OutputDebugString on Windows, printf on Linux)
    void DebugOutput(const char* message);

    // Message box (for critical errors)
    void ShowMessageBox(const char* title, const char* message, bool isError = false);

} // namespace Platform
} // namespace USE