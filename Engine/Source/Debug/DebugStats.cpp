// ============================================================
// DebugStats.cpp
// ============================================================
#include "DebugStats.h"
#include "Core/Logger.h"

namespace USE {
    DebugStats& DebugStats::Get() { static DebugStats instance; return instance; }

    void DebugStats::SetStat(const std::string& name, const std::string& value) { m_stats[name] = value; }
    void DebugStats::SetStat(const std::string& name, int value) { SetStat(name, std::to_string(value)); }
    void DebugStats::SetStat(const std::string& name, float value) { SetStat(name, std::to_string(value)); }
    void DebugStats::ClearStat(const std::string& name) { m_stats.erase(name); }
    const std::string& DebugStats::GetStat(const std::string& name) const {
        static std::string empty;
        auto it = m_stats.find(name);
        return (it != m_stats.end()) ? it->second : empty;
    }

    void DebugStats::Draw() {
        if (m_stats.empty()) return;
        std::string output = "=== Debug Stats ===\n";
        for (const auto& p : m_stats) output += p.first + ": " + p.second + "\n";
        // In a real engine, render this text overlay.
        USE_LOG_INFO("%s", output.c_str());
    }
}