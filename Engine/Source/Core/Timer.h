// ============================================================
// Ultimate Source Engine - High-Resolution Timer
// ============================================================
//
// Provides high-precision timing functionality for game loops,
// profiling, and general time measurement.
// ============================================================

#pragma once

#include "stdafx.h"

namespace USE {

    // -----------------------------------------------------------------
    // Simple stopwatch timer
    // -----------------------------------------------------------------
    class Timer {
    public:
        Timer();
        ~Timer() = default;

        // Start/reset the timer
        void Start();
        void Reset();

        // Stop the timer (pause)
        void Stop();

        // Resume after stop
        void Resume();

        // Get elapsed time (since start) in various units
        double GetElapsedSeconds() const;
        double GetElapsedMilliseconds() const;
        double GetElapsedMicroseconds() const;
        uint64 GetElapsedTicks() const;  // platform-specific ticks (microseconds)

        // Check if timer is running
        bool IsRunning() const { return m_running; }

        // Get the current high-resolution time in microseconds (static)
        static uint64 GetCurrentTimeMicroseconds();

        // Sleep for specified duration
        static void SleepSeconds(double seconds);
        static void SleepMilliseconds(double ms);
        static void SleepMicroseconds(uint64 us);

    private:
        uint64 m_startTime;   // start time in microseconds
        uint64 m_pausedTime;  // total accumulated paused time
        uint64 m_pausedAt;    // time when paused (for resume)
        bool   m_running;
    };

    // -----------------------------------------------------------------
    // Frame timer designed for game loops
    // -----------------------------------------------------------------
    class FrameTimer {
    public:
        FrameTimer();
        ~FrameTimer() = default;

        // Call at the beginning of each frame
        void Tick();

        // Get delta time for current frame (seconds)
        float GetDeltaTime() const { return m_deltaTime; }

        // Get smoothed FPS (frames per second)
        float GetFPS() const { return m_fps; }

        // Get total elapsed time since first Tick()
        float GetTotalTime() const { return m_totalTime; }

        // Get frame count
        uint64 GetFrameCount() const { return m_frameCount; }

        // Set target frame rate (0 = unlimited)
        void SetTargetFPS(uint32 fps);

        // Get target frame time in microseconds
        uint64 GetTargetFrameTime() const { return m_targetFrameTime; }

        // Limit frame rate based on target (call after Tick())
        void LimitFrameRate();

    private:
        Timer  m_timer;
        uint64 m_lastFrameTime;          // time of last frame in microseconds
        float  m_deltaTime;               // time since last frame in seconds
        float  m_totalTime;                // total elapsed time in seconds
        float  m_fps;                      // current FPS
        uint64 m_frameCount;                // number of frames elapsed

        // FPS smoothing
        float  m_fpsUpdateInterval;        // how often to update FPS (seconds)
        float  m_fpsTimer;                  // time since last FPS update
        uint64 m_frameCountSinceFPSUpdate; // frames since last FPS update

        // Frame limiting
        uint64 m_targetFrameTime;           // desired microseconds per frame (0 = unlimited)
    };

} // namespace USE