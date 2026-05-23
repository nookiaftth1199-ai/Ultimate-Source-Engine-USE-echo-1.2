// ============================================================
// Ultimate Source Engine - Configuration Manager
// ============================================================
//
// Manages configuration files with key=value pairs, optionally
// grouped into sections. Supports integration with console variables
// for automatic loading and saving.
// ============================================================

#pragma once

#include "stdafx.h"
#include <map>
#include <string>
#include <vector>

namespace USE {

    // Forward declaration
    class ConsoleVariableBase;

    class ConfigManager {
    public:
        ConfigManager();
        ~ConfigManager();

        // Load configuration from a file (clears current settings)
        bool Load(const std::string& filename);

        // Save current configuration to a file
        bool Save(const std::string& filename) const;

        // Get/set raw string values (with optional section)
        std::string GetString(const std::string& key, const std::string& section = "") const;
        void SetString(const std::string& key, const std::string& value, const std::string& section = "");

        // Typed getters with defaults
        int         GetInt(const std::string& key, int defaultValue = 0, const std::string& section = "") const;
        float       GetFloat(const std::string& key, float defaultValue = 0.0f, const std::string& section = "") const;
        bool        GetBool(const std::string& key, bool defaultValue = false, const std::string& section = "") const;

        // Check if a key exists
        bool HasKey(const std::string& key, const std::string& section = "") const;

        // Remove a key
        void RemoveKey(const std::string& key, const std::string& section = "");

        // Clear all settings
        void Clear();

        // Get all keys in a section (for enumeration)
        std::vector<std::string> GetKeys(const std::string& section = "") const;

        // Get all sections
        std::vector<std::string> GetSections() const;

        // CVar integration: register a CVar to be automatically synced
        void RegisterCVar(const std::string& name, ConsoleVariableBase* cvar);
        void UnregisterCVar(const std::string& name);

        // Update CVar values from the current config data
        void UpdateCVarsFromConfig();

        // Update config data from registered CVars (for saving)
        void UpdateConfigFromCVars();

    private:
        using SectionMap = std::map<std::string, std::string>;
        using ConfigData = std::map<std::string, SectionMap>;

        ConfigData m_data;
        std::map<std::string, ConsoleVariableBase*> m_cvarMap;

        // Parse a line from a config file, filling section and key/value.
        // Returns true if a key/value pair was parsed.
        bool ParseLine(const std::string& line, std::string& section, std::string& key, std::string& value);
    };

} // namespace USE