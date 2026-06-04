// ============================================================
// PerformanceMonitor.h
// ============================================================
#pragma once
#include <string>
#include <chrono>

namespace USE {
    class PerformanceMonitor {
    public:
        static PerformanceMonitor& Get();

        void BeginFrame();
        void EndFrame();
        void AddSample(const std::string& name, double microseconds);
        void Reset();

        double GetFrameTime() const { return m_frameTime; }
        double GetFPS() const { return m_fps; }
        void Update();

        void Draw(); // output to log or ImGui

    private:
        PerformanceMonitor() = default;
        std::chrono::high_resolution_clock::time_point m_frameStart;
        double m_frameTime = 0.0;
        double m_fps = 0.0;
        int m_frameCount = 0;
        double m_timeAcc = 0.0;
    };
}