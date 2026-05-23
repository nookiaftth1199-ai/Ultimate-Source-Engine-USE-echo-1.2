// ============================================================
// Ultimate Source Engine - Version Information
// ============================================================
//
// This file defines the current engine version, codename,
// build date, and other metadata.
// ============================================================

#pragma once

#ifndef USE_ENGINE_VERSION_MAJOR
#define USE_ENGINE_VERSION_MAJOR 1
#endif

#ifndef USE_ENGINE_VERSION_MINOR
#define USE_ENGINE_VERSION_MINOR 2
#endif

#ifndef USE_ENGINE_VERSION_PATCH
#define USE_ENGINE_VERSION_PATCH 0
#endif

#define USE_ENGINE_VERSION_STRING "1.2.0"
#define USE_ENGINE_CODENAME "Echo"

#define USE_ENGINE_NAME "Ultimate Source Engine"
#define USE_ENGINE_FULLNAME USE_ENGINE_NAME " " USE_ENGINE_VERSION_STRING " \"" USE_ENGINE_CODENAME "\""

// Build date (set by preprocessor)
#define USE_ENGINE_BUILD_DATE __DATE__
#define USE_ENGINE_BUILD_TIME __TIME__

// Compiler detection (simplified)
#if defined(_MSC_VER)
    #if _MSC_VER == 1900
        #define USE_ENGINE_COMPILER "MSVC 2015"
    #elif _MSC_VER == 1910
        #define USE_ENGINE_COMPILER "MSVC 2017"
    #elif _MSC_VER >= 1920
        #define USE_ENGINE_COMPILER "MSVC 2019/2022"
    #else
        #define USE_ENGINE_COMPILER "MSVC (unknown)"
    #endif
#elif defined(__clang__)
    #define USE_ENGINE_COMPILER "Clang"
#elif defined(__GNUC__)
    #define USE_ENGINE_COMPILER "GCC"
#else
    #define USE_ENGINE_COMPILER "Unknown"
#endif

// Platform
#if defined(_WIN32) || defined(_WIN64)
    #define USE_ENGINE_PLATFORM "Windows"
#elif defined(__linux__)
    #define USE_ENGINE_PLATFORM "Linux"
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define USE_ENGINE_PLATFORM "macOS"
    #else
        #define USE_ENGINE_PLATFORM "iOS"
    #endif
#else
    #define USE_ENGINE_PLATFORM "Unknown"
#endif

// Architecture
#if defined(_M_X64) || defined(__x86_64__)
    #define USE_ENGINE_ARCH "64-bit"
#elif defined(_M_IX86) || defined(__i386__)
    #define USE_ENGINE_ARCH "32-bit"
#elif defined(__aarch64__)
    #define USE_ENGINE_ARCH "ARM64"
#else
    #define USE_ENGINE_ARCH "Unknown"
#endif

// Helper macro to get full version string
#define USE_ENGINE_GET_VERSION_STRING() \
    USE_ENGINE_FULLNAME " (" USE_ENGINE_PLATFORM " " USE_ENGINE_ARCH ", " USE_ENGINE_COMPILER ", " USE_ENGINE_BUILD_DATE " " USE_ENGINE_BUILD_TIME ")"

// For console / logging
inline const char* GetEngineVersionString() {
    return USE_ENGINE_GET_VERSION_STRING();
}

// Engine version check (compile-time)
#define USE_ENGINE_VERSION_AT_LEAST(major, minor, patch) \
    (USE_ENGINE_VERSION_MAJOR > (major) || \
     (USE_ENGINE_VERSION_MAJOR == (major) && USE_ENGINE_VERSION_MINOR > (minor)) || \
     (USE_ENGINE_VERSION_MAJOR == (major) && USE_ENGINE_VERSION_MINOR == (minor) && USE_ENGINE_VERSION_PATCH >= (patch)))