// ============================================================
// Ultimate Source Engine - Console Variables
// ============================================================
//
// Defines console variables (CVars) that can be read and written
// from the developer console. Supports various types, flags,
// and change callbacks.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <functional>
#include <sstream>
#include <iomanip>

namespace USE {

    // -----------------------------------------------------------------
    // CVar flags
    // -----------------------------------------------------------------
    enum class CVarFlags {
        None        = 0,
        Archive     = 1 << 0,   // Save to config file
        ReadOnly    = 1 << 1,   // Cannot be changed at runtime
        Cheat       = 1 << 2,   // Requires sv_cheats = 1
        Developer   = 1 << 3,   // Only visible in developer mode
        Hidden      = 1 << 4,   // Hidden from cvarlist
        Init        = 1 << 5,   // Can only be set at initialization
        Password    = 1 << 6,   // Password field (hide input)
    };

    inline CVarFlags operator|(CVarFlags a, CVarFlags b) {
        return static_cast<CVarFlags>(static_cast<int>(a) | static_cast<int>(b));
    }
    inline CVarFlags operator&(CVarFlags a, CVarFlags b) {
        return static_cast<CVarFlags>(static_cast<int>(a) & static_cast<int>(b));
    }

    // -----------------------------------------------------------------
    // Base class for all console variables
    // -----------------------------------------------------------------
    class ConsoleVariableBase {
    public:
        using Callback = std::function<void(ConsoleVariableBase*)>;

        ConsoleVariableBase(const std::string& name,
                            const std::string& description,
                            CVarFlags flags = CVarFlags::None);
        virtual ~ConsoleVariableBase() = default;

        // Accessors
        const std::string& GetName() const { return m_name; }
        const std::string& GetDescription() const { return m_description; }
        CVarFlags GetFlags() const { return m_flags; }
        bool HasFlag(CVarFlags flag) const {
            return (static_cast<int>(m_flags) & static_cast<int>(flag)) != 0;
        }

        // Get/set as string (for console I/O)
        virtual std::string GetString() const = 0;
        virtual void SetString(const std::string& value) = 0;

        // Get default string (for reset)
        virtual std::string GetDefaultString() const = 0;
        virtual void ResetToDefault() = 0;

        // Change callback
        void SetCallback(Callback callback) { m_callback = callback; }
        void NotifyChanged() { if (m_callback) m_callback(this); }

        // Type name for debugging
        virtual const char* GetTypeName() const = 0;

    protected:
        std::string m_name;
        std::string m_description;
        CVarFlags   m_flags;
        Callback    m_callback;
    };

    // -----------------------------------------------------------------
    // Typed console variable
    // -----------------------------------------------------------------
    template<typename T>
    class ConsoleVariable : public ConsoleVariableBase {
    public:
        ConsoleVariable(const std::string& name, const T& defaultValue,
                        const std::string& description = "",
                        CVarFlags flags = CVarFlags::None);

        // Get/set value directly
        T GetValue() const { return m_value; }
        void SetValue(const T& value);

        // Overrides from base
        std::string GetString() const override;
        void SetString(const std::string& value) override;
        std::string GetDefaultString() const override;
        void ResetToDefault() override;
        const char* GetTypeName() const override;

    private:
        T m_value;
        T m_defaultValue;
    };

    // -----------------------------------------------------------------
    // String specialization (partial implementation)
    // -----------------------------------------------------------------
    template<>
    std::string ConsoleVariable<std::string>::GetString() const;

    template<>
    void ConsoleVariable<std::string>::SetString(const std::string& value);

    // -----------------------------------------------------------------
    // Inline implementations (for simple types)
    // -----------------------------------------------------------------
    template<typename T>
    ConsoleVariable<T>::ConsoleVariable(const std::string& name, const T& defaultValue,
                                        const std::string& description, CVarFlags flags)
        : ConsoleVariableBase(name, description, flags)
        , m_value(defaultValue)
        , m_defaultValue(defaultValue)
    {}

    template<typename T>
    void ConsoleVariable<T>::SetValue(const T& value) {
        if (HasFlag(CVarFlags::ReadOnly)) return;
        m_value = value;
        NotifyChanged();
    }

    template<typename T>
    std::string ConsoleVariable<T>::GetString() const {
        std::ostringstream oss;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (m_value ? "1" : "0");
        } else if constexpr (std::is_same_v<T, float>) {
            oss << std::fixed << std::setprecision(4) << m_value;
        } else {
            oss << m_value;
        }
        return oss.str();
    }

    template<typename T>
    void ConsoleVariable<T>::SetString(const std::string& value) {
        std::istringstream iss(value);
        T newValue;
        if (iss >> newValue) {
            SetValue(newValue);
        }
    }

    template<typename T>
    std::string ConsoleVariable<T>::GetDefaultString() const {
        std::ostringstream oss;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (m_defaultValue ? "1" : "0");
        } else if constexpr (std::is_same_v<T, float>) {
            oss << std::fixed << std::setprecision(4) << m_defaultValue;
        } else {
            oss << m_defaultValue;
        }
        return oss.str();
    }

    template<typename T>
    void ConsoleVariable<T>::ResetToDefault() {
        if (HasFlag(CVarFlags::ReadOnly)) return;
        m_value = m_defaultValue;
        NotifyChanged();
    }

    template<typename T>
    const char* ConsoleVariable<T>::GetTypeName() const {
        if constexpr (std::is_same_v<T, int>) return "int";
        if constexpr (std::is_same_v<T, float>) return "float";
        if constexpr (std::is_same_v<T, bool>) return "bool";
        if constexpr (std::is_same_v<T, std::string>) return "string";
        return "unknown";
    }

} // namespace USE