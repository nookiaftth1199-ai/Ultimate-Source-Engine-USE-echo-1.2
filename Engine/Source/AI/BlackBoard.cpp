// ============================================================
// Ultimate Source Engine - Blackboard Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "Blackboard.h"

namespace USE {

    // -----------------------------------------------------------------
    // Value constructors/destructor
    // -----------------------------------------------------------------
    Blackboard::Value::Value() : type(ValueType::None) {}

    Blackboard::Value::Value(int val) : type(ValueType::Int) { data.intVal = val; }
    Blackboard::Value::Value(float val) : type(ValueType::Float) { data.floatVal = val; }
    Blackboard::Value::Value(bool val) : type(ValueType::Bool) { data.boolVal = val; }
    Blackboard::Value::Value(const Vector3& val) : type(ValueType::Vector3) {
        data.vec3Val.x = val.x;
        data.vec3Val.y = val.y;
        data.vec3Val.z = val.z;
    }
    Blackboard::Value::Value(const std::string& val) : type(ValueType::String) {
        data.strVal = new std::string(val);
    }

    Blackboard::Value::Value(const Value& other) : type(other.type) {
        switch (type) {
            case ValueType::Int:    data.intVal = other.data.intVal; break;
            case ValueType::Float:  data.floatVal = other.data.floatVal; break;
            case ValueType::Bool:   data.boolVal = other.data.boolVal; break;
            case ValueType::Vector3: data.vec3Val = other.data.vec3Val; break;
            case ValueType::String:  data.strVal = new std::string(*other.data.strVal); break;
            default: break;
        }
    }

    Blackboard::Value::~Value() {
        if (type == ValueType::String && data.strVal) {
            delete data.strVal;
        }
    }

    Blackboard::Value& Blackboard::Value::operator=(const Value& other) {
        if (this == &other) return *this;
        // Clean up existing string if needed
        if (type == ValueType::String && data.strVal) {
            delete data.strVal;
        }
        type = other.type;
        switch (type) {
            case ValueType::Int:    data.intVal = other.data.intVal; break;
            case ValueType::Float:  data.floatVal = other.data.floatVal; break;
            case ValueType::Bool:   data.boolVal = other.data.boolVal; break;
            case ValueType::Vector3: data.vec3Val = other.data.vec3Val; break;
            case ValueType::String:  data.strVal = new std::string(*other.data.strVal); break;
            default: break;
        }
        return *this;
    }

    // -----------------------------------------------------------------
    // Blackboard
    // -----------------------------------------------------------------
    Blackboard::Blackboard() {}
    Blackboard::~Blackboard() { Clear(); }

    void Blackboard::Clear() {
        m_values.clear();
    }

    void Blackboard::SetInt(const std::string& key, int value) {
        m_values[key] = Value(value);
    }

    void Blackboard::SetFloat(const std::string& key, float value) {
        m_values[key] = Value(value);
    }

    void Blackboard::SetBool(const std::string& key, bool value) {
        m_values[key] = Value(value);
    }

    void Blackboard::SetVector3(const std::string& key, const Vector3& value) {
        m_values[key] = Value(value);
    }

    void Blackboard::SetString(const std::string& key, const std::string& value) {
        m_values[key] = Value(value);
    }

    bool Blackboard::GetInt(const std::string& key, int& outValue) const {
        auto it = m_values.find(key);
        if (it != m_values.end() && it->second.type == ValueType::Int) {
            outValue = it->second.data.intVal;
            return true;
        }
        return false;
    }

    bool Blackboard::GetFloat(const std::string& key, float& outValue) const {
        auto it = m_values.find(key);
        if (it != m_values.end() && it->second.type == ValueType::Float) {
            outValue = it->second.data.floatVal;
            return true;
        }
        return false;
    }

    bool Blackboard::GetBool(const std::string& key, bool& outValue) const {
        auto it = m_values.find(key);
        if (it != m_values.end() && it->second.type == ValueType::Bool) {
            outValue = it->second.data.boolVal;
            return true;
        }
        return false;
    }

    bool Blackboard::GetVector3(const std::string& key, Vector3& outValue) const {
        auto it = m_values.find(key);
        if (it != m_values.end() && it->second.type == ValueType::Vector3) {
            outValue.x = it->second.data.vec3Val.x;
            outValue.y = it->second.data.vec3Val.y;
            outValue.z = it->second.data.vec3Val.z;
            return true;
        }
        return false;
    }

    bool Blackboard::GetString(const std::string& key, std::string& outValue) const {
        auto it = m_values.find(key);
        if (it != m_values.end() && it->second.type == ValueType::String) {
            outValue = *it->second.data.strVal;
            return true;
        }
        return false;
    }

    int Blackboard::GetInt(const std::string& key, int defaultValue) const {
        int val;
        return GetInt(key, val) ? val : defaultValue;
    }

    float Blackboard::GetFloat(const std::string& key, float defaultValue) const {
        float val;
        return GetFloat(key, val) ? val : defaultValue;
    }

    bool Blackboard::GetBool(const std::string& key, bool defaultValue) const {
        bool val;
        return GetBool(key, val) ? val : defaultValue;
    }

    Vector3 Blackboard::GetVector3(const std::string& key, const Vector3& defaultValue) const {
        Vector3 val;
        return GetVector3(key, val) ? val : defaultValue;
    }

    std::string Blackboard::GetString(const std::string& key, const std::string& defaultValue) const {
        std::string val;
        return GetString(key, val) ? val : defaultValue;
    }

    bool Blackboard::HasKey(const std::string& key) const {
        return m_values.find(key) != m_values.end();
    }

    void Blackboard::RemoveKey(const std::string& key) {
        m_values.erase(key);
    }

} // namespace USE