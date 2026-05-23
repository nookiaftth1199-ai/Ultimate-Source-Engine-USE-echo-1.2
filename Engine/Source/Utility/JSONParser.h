// ============================================================
// JSONParser.h
// ============================================================
#pragma once
#include "json.hpp"
using json = nlohmann::json;

namespace USE {
    class JSONParser {
    public:
        static bool ParseFile(const std::string& filename, json& out);
        static bool ParseString(const std::string& content, json& out);
        static bool SaveToFile(const json& data, const std::string& filename);
    };
}