// ============================================================
// PerformanceMonitor.cpp
// ============================================================
#include "PerformanceMonitor.h"
#include "Logger.h"

namespace USE {
    PerformanceMonitor& PerformanceMonitor::Get() { static PerformanceMonitor instance; return instance; }

    void PerformanceMonitor::BeginFrame() {
        m_frameStart = std::chrono::high_resolution_clock::now();
    }

    void PerformanceMonitor::EndFrame() {
        auto end = std::chrono::high_resolution_clock::now();
        m_frameTime = std::chrono::duration<double, std::micro>(end - m_frameStart).count() / 1000.0; // ms
        Update();
    }

    void PerformanceMonitor::Update() {
        m_frameCount++;
        m_timeAcc += m_frameTime;
        if (m_timeAcc >= 1000.0) { // every second
            m_fps = m_frameCount * 1000.0 / m_timeAcc;
            m_frameCount = 0;
            m_timeAcc = 0.0;
        }
    }

    void PerformanceMonitor::Reset() { m_frameCount = 0; m_timeAcc = 0.0; m_fps = 0.0; }

    void PerformanceMonitor::Draw() {
        USE_LOG_INFO("Performance: Frame %.2f ms, FPS %.1f", m_frameTime, m_fps);
    }

    void PerformanceMonitor::AddSample(const std::string& name, double microseconds) {
        // Can be extended to track custom samples.
    }
}