// ============================================================
// JSONWrapper.cpp
// ============================================================
#include "JSONWrapper.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include <fstream>

namespace USE {
    bool JSONWrapper::Parse(const std::string& content, JSON& out) {
        try {
            out = JSON::parse(content);
            return true;
        } catch (const std::exception& e) {
            USE_LOG_ERROR("JSON parse error: %s", e.what());
            return false;
        }
    }

    bool JSONWrapper::LoadFromFile(const std::string& filename, JSON& out) {
        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;
        auto file = fs->OpenFile(filename, FILE_READ | FILE_TEXT);
        if (!file) return false;
        std::string content;
        file->ReadAll(content);
        file->Close();
        return Parse(content, out);
    }

    bool JSONWrapper::SaveToFile(const JSON& data, const std::string& filename) {
        FileSystem* fs = FileSystem::Get();
        if (!fs) return false;
        auto file = fs->OpenFile(filename, FILE_WRITE | FILE_TEXT | FILE_TRUNCATE);
        if (!file) return false;
        file->WriteLine(data.dump(4));
        file->Close();
        return true;
    }
}