// ============================================================
// Ultimate Source Engine - Blackboard
//============================================================
//
// Key-value store used by AI controllers to share data.
// Supports common types: int, float, bool, Vector3, std::string.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector3.h"
#include <unordered_map>
#include <string>

namespace USE {

    class Blackboard {
    public:
        Blackboard();
        ~Blackboard();

        // Clear all entries
        void Clear();

        // Set values
        void SetInt(const std::string& key, int value);
        void SetFloat(const std::string& key, float value);
        void SetBool(const std::string& key, bool value);
        void SetVector3(const std::string& key, const Vector3& value);
        void SetString(const std::string& key, const std::string& value);

        // Get values (return true if key exists and type matches)
        bool GetInt(const std::string& key, int& outValue) const;
        bool GetFloat(const std::string& key, float& outValue) const;
        bool GetBool(const std::string& key, bool& outValue) const;
        bool GetVector3(const std::string& key, Vector3& outValue) const;
        bool GetString(const std::string& key, std::string& outValue) const;

        // Convenience: get with default (if key missing or type mismatch, returns default)
        int     GetInt(const std::string& key, int defaultValue = 0) const;
        float   GetFloat(const std::string& key, float defaultValue = 0.0f) const;
        bool    GetBool(const std::string& key, bool defaultValue = false) const;
        Vector3 GetVector3(const std::string& key, const Vector3& defaultValue = Vector3::Zero) const;
        std::string GetString(const std::string& key, const std::string& defaultValue = "") const;

        // Check if key exists (any type)
        bool HasKey(const std::string& key) const;

        // Remove a key
        void RemoveKey(const std::string& key);

    private:
        enum class ValueType {
            None,
            Int,
            Float,
            Bool,
            Vector3,
            String
        };

        struct Value {
            ValueType type;
            union {
                int intVal;
                float floatVal;
                bool boolVal;
                struct { float x, y, z; } vec3Val;
                std::string* strVal; // dynamically allocated
            } data;

            Value();
            Value(int val);
            Value(float val);
            Value(bool val);
            Value(const Vector3& val);
            Value(const std::string& val);
            Value(const Value& other);
            ~Value();

            Value& operator=(const Value& other);
        };

        std::unordered_map<std::string, Value> m_values;
    };

} // namespace USE