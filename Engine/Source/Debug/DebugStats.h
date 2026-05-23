// ============================================================
// DebugStats.h
// ============================================================
#pragma once
#include <string>
#include <map>
#include <functional>

namespace USE {
    class DebugStats {
    public:
        static DebugStats& Get();

        void SetStat(const std::string& name, const std::string& value);
        void SetStat(const std::string& name, int value);
        void SetStat(const std::string& name, float value);
        void ClearStat(const std::string& name);
        const std::string& GetStat(const std::string& name) const;

        void Draw();

    private:
        DebugStats() = default;
        std::map<std::string, std::string> m_stats;
    };
}