// ============================================================
// Ultimate Source Engine - Configuration Manager Implementation
// ============================================================

#include "stdafx.h"
#include "ConfigManager.h"
#include "Core/FileSystem.h"
#include "Core/Logger.h"
#include "Utility/StringUtils.h"

namespace USE {

    ConfigManager::ConfigManager()
    {
    }

    ConfigManager::~ConfigManager()
    {
    }

    bool ConfigManager::Load(const std::string& filename)
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ConfigManager: FileSystem not available");
            return false;
        }

        auto file = fs->OpenFile(filename, FILE_READ | FILE_TEXT);
        if (!file) {
            USE_LOG_WARN("ConfigManager: Could not open config file: %s", filename.c_str());
            return false;
        }

        Clear();

        std::string line;
        std::string currentSection;
        while (file->ReadLine(line)) {
            std::string section, key, value;
            if (ParseLine(line, section, key, value)) {
                if (!section.empty()) {
                    currentSection = section;
                }
                if (!key.empty()) {
                    m_data[currentSection][key] = value;
                }
            }
        }

        file->Close();
        USE_LOG_INFO("ConfigManager: Loaded config from %s", filename.c_str());
        return true;
    }

    bool ConfigManager::Save(const std::string& filename) const
    {
        FileSystem* fs = FileSystem::Get();
        if (!fs) {
            USE_LOG_ERROR("ConfigManager: FileSystem not available");
            return false;
        }

        auto file = fs->OpenFile(filename, FILE_WRITE | FILE_TEXT | FILE_TRUNCATE);
        if (!file) {
            USE_LOG_ERROR("ConfigManager: Cannot write config file: %s", filename.c_str());
            return false;
        }

        // Write global section first (empty section name)
        auto globalIt = m_data.find("");
        if (globalIt != m_data.end()) {
            for (const auto& kv : globalIt->second) {
                file->WriteLine(kv.first + " = " + kv.second);
            }
            file->WriteLine(""); // blank line after global section
        }

        // Write other sections
        for (const auto& sectionPair : m_data) {
            if (sectionPair.first.empty()) continue;

            file->WriteLine("[" + sectionPair.first + "]");
            for (const auto& kv : sectionPair.second) {
                file->WriteLine(kv.first + " = " + kv.second);
            }
            file->WriteLine(""); // blank line between sections
        }

        file->Close();
        USE_LOG_INFO("ConfigManager: Saved config to %s", filename.c_str());
        return true;
    }

    std::string ConfigManager::GetString(const std::string& key, const std::string& section) const
    {
        auto secIt = m_data.find(section);
        if (secIt != m_data.end()) {
            auto keyIt = secIt->second.find(key);
            if (keyIt != secIt->second.end()) {
                return keyIt->second;
            }
        }
        return "";
    }

    void ConfigManager::SetString(const std::string& key, const std::string& value, const std::string& section)
    {
        m_data[section][key] = value;
    }

    int ConfigManager::GetInt(const std::string& key, int defaultValue, const std::string& section) const
    {
        std::string val = GetString(key, section);
        if (val.empty()) return defaultValue;
        return StringUtils::ToInt(val);
    }

    float ConfigManager::GetFloat(const std::string& key, float defaultValue, const std::string& section) const
    {
        std::string val = GetString(key, section);
        if (val.empty()) return defaultValue;
        return StringUtils::ToFloat(val);
    }

    bool ConfigManager::GetBool(const std::string& key, bool defaultValue, const std::string& section) const
    {
        std::string val = GetString(key, section);
        if (val.empty()) return defaultValue;
        return StringUtils::ToBool(val);
    }

    bool ConfigManager::HasKey(const std::string& key, const std::string& section) const
    {
        auto secIt = m_data.find(section);
        return (secIt != m_data.end()) && (secIt->second.find(key) != secIt->second.end());
    }

    void ConfigManager::RemoveKey(const std::string& key, const std::string& section)
    {
        auto secIt = m_data.find(section);
        if (secIt != m_data.end()) {
            secIt->second.erase(key);
        }
    }

    void ConfigManager::Clear()
    {
        m_data.clear();
    }

    std::vector<std::string> ConfigManager::GetKeys(const std::string& section) const
    {
        std::vector<std::string> keys;
        auto secIt = m_data.find(section);
        if (secIt != m_data.end()) {
            for (const auto& kv : secIt->second) {
                keys.push_back(kv.first);
            }
        }
        return keys;
    }

    std::vector<std::string> ConfigManager::GetSections() const
    {
        std::vector<std::string> sections;
        for (const auto& pair : m_data) {
            if (!pair.first.empty()) {
                sections.push_back(pair.first);
            }
        }
        return sections;
    }

    void ConfigManager::RegisterCVar(const std::string& name, ConsoleVariableBase* cvar)
    {
        m_cvarMap[name] = cvar;
    }

    void ConfigManager::UnregisterCVar(const std::string& name)
    {
        m_cvarMap.erase(name);
    }

    void ConfigManager::UpdateCVarsFromConfig()
    {
        for (const auto& kv : m_cvarMap) {
            const std::string& name = kv.first;
            ConsoleVariableBase* cvar = kv.second;
            std::string val = GetString(name); // assume no section for CVars
            if (!val.empty()) {
                cvar->SetString(val);
            }
        }
    }

    void ConfigManager::UpdateConfigFromCVars()
    {
        for (const auto& kv : m_cvarMap) {
            const std::string& name = kv.first;
            ConsoleVariableBase* cvar = kv.second;
            SetString(name, cvar->GetString());
        }
    }

    bool ConfigManager::ParseLine(const std::string& line, std::string& section, std::string& key, std::string& value)
    {
        std::string trimmed = StringUtils::Trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
            return false; // comment or empty line
        }

        // Section header: [SectionName]
        if (trimmed[0] == '[') {
            size_t close = trimmed.find(']');
            if (close != std::string::npos) {
                section = trimmed.substr(1, close - 1);
                section = StringUtils::Trim(section);
                key.clear();
                value.clear();
                return true; // section line, no key/value
            }
        }

        // Key = value
        size_t eq = trimmed.find('=');
        if (eq != std::string::npos) {
            key = StringUtils::Trim(trimmed.substr(0, eq));
            value = StringUtils::Trim(trimmed.substr(eq + 1));
            return true;
        }

        return false; // unrecognized line
    }

} // namespace USE