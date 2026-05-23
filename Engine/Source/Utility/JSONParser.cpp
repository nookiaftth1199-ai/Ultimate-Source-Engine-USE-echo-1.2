// ============================================================
// JSONParser.cpp
// ============================================================
#include "JSONParser.h"
#include "FileUtils.h"
#include <fstream>

namespace USE {
    bool JSONParser::ParseFile(const std::string& filename, json& out) {
        std::string content;
        if (!FileUtils::ReadTextFile(filename, content)) return false;
        try { out = json::parse(content); return true; }
        catch (...) { return false; }
    }
    bool JSONParser::ParseString(const std::string& content, json& out) {
        try { out = json::parse(content); return true; }
        catch (...) { return false; }
    }
    bool JSONParser::SaveToFile(const json& data, const std::string& filename) {
        std::ofstream f(filename);
        if (!f.is_open()) return false;
        f << data.dump(4);
        return true;
    }
}