// ============================================================
// JSONWrapper.h
// ============================================================
#pragma once
#include "json.hpp"

namespace USE {
    using JSON = nlohmann::json;

    class JSONWrapper {
    public:
        static bool Parse(const std::string& content, JSON& out);
        static bool LoadFromFile(const std::string& filename, JSON& out);
        static bool SaveToFile(const JSON& data, const std::string& filename);
    };
}