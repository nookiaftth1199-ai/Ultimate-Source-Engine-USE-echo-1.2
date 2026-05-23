// ============================================================
// Ultimate Source Engine - Logger Implementation
// ============================================================

#include "stdafx.h"
#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>

namespace USE {

    Logger::Logger()
        : m_minLevel(LogLevel::Info)
        , m_consoleOutput(true)
        , m_fileOutput(true)
        , m_initialized(false)
    {
    }

    Logger::~Logger()
    {
        Shutdown();
    }

    Logger& Logger::Get()
    {
        static Logger instance;
        return instance;
    }

    bool Logger::Initialize(const std::string& logFile)
    {
        if (m_initialized) return true;

        if (m_fileOutput) {
            m_logFile.open(logFile.c_str(), std::ios::out | std::ios::trunc);
            if (!m_logFile.is_open()) {
                // Fallback: disable file output
                m_fileOutput = false;
            }
        }

        m_initialized = true;
        return true;
    }

    void Logger::Shutdown()
    {
        if (m_logFile.is_open()) {
            m_logFile.close();
        }
        m_initialized = false;
    }

    void Logger::Log(LogLevel level, const char* file, int line, const char* format, ...)
    {
        if (!m_initialized) return;
        if (level < m_minLevel) return;

        // Get current time
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char timeBuf[20];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", tm_info);

        // Format the message
        char message[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(message, sizeof(message), format, args);
        va_end(args);

        // Build log line
        char fullLine[2048];
        snprintf(fullLine, sizeof(fullLine), "[%s] [%s] %s (%s:%d)",
                 timeBuf, LevelToString(level), message, file, line);

        // Output to console
        if (m_consoleOutput) {
            // Color coding for console (Windows)
#ifdef _WIN32
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // default white
            switch (level) {
                case LogLevel::Debug:   color = FOREGROUND_INTENSITY; break;
                case LogLevel::Info:    color = FOREGROUND_GREEN | FOREGROUND_BLUE; break;
                case LogLevel::Warning: color = FOREGROUND_RED | FOREGROUND_GREEN; break;
                case LogLevel::Error:   color = FOREGROUND_RED; break;
            }
            SetConsoleTextAttribute(hConsole, color);
            printf("%s\n", fullLine);
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
            // On non‑Windows, just print plain
            printf("%s\n", fullLine);
#endif
        }

        // Output to file
        if (m_fileOutput && m_logFile.is_open()) {
            m_logFile << fullLine << std::endl;
            m_logFile.flush();
        }
    }

    const char* Logger::LevelToString(LogLevel level)
    {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            default:                return "UNKNOWN";
        }
    }

} // namespace USE