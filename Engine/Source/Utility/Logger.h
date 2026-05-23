// ============================================================
// Ultimate Source Engine - Logger
// ============================================================
//
// Provides logging with different severity levels.
// Supports console output and file logging.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <fstream>

namespace USE {

    // Log severity levels
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger {
    public:
        // Get the singleton instance.
        static Logger& Get();

        // Initialize the logger (open log file, set console output, etc.)
        bool Initialize(const std::string& logFile = "engine.log");
        void Shutdown();

        // Set the minimum log level (messages below this level are ignored)
        void SetLevel(LogLevel level) { m_minLevel = level; }

        // Log a message with a specific level and optional file/line info.
        void Log(LogLevel level, const char* file, int line, const char* format, ...);

        // Enable/disable console output.
        void SetConsoleOutput(bool enable) { m_consoleOutput = enable; }

        // Enable/disable file output.
        void SetFileOutput(bool enable) { m_fileOutput = enable; }

    private:
        Logger();
        ~Logger();

        LogLevel m_minLevel;
        std::ofstream m_logFile;
        bool m_consoleOutput;
        bool m_fileOutput;
        bool m_initialized;

        // Helper to get level as string.
        static const char* LevelToString(LogLevel level);
    };

} // namespace USE

// -----------------------------------------------------------------
// Convenience macros
// -----------------------------------------------------------------
#define USE_LOG_DEBUG(...)   USE::Logger::Get().Log(USE::LogLevel::Debug,   __FILE__, __LINE__, __VA_ARGS__)
#define USE_LOG_INFO(...)    USE::Logger::Get().Log(USE::LogLevel::Info,    __FILE__, __LINE__, __VA_ARGS__)
#define USE_LOG_WARN(...)    USE::Logger::Get().Log(USE::LogLevel::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define USE_LOG_ERROR(...)   USE::Logger::Get().Log(USE::LogLevel::Error,   __FILE__, __LINE__, __VA_ARGS__)