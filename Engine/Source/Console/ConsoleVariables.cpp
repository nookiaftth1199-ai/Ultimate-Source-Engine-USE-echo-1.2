// ============================================================
// Ultimate Source Engine - Console Variables Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleVariables.h"

namespace USE {

    // -----------------------------------------------------------------
    // ConsoleVariableBase constructor
    // -----------------------------------------------------------------
    ConsoleVariableBase::ConsoleVariableBase(const std::string& name,
                                             const std::string& description,
                                             CVarFlags flags)
        : m_name(name)
        , m_description(description)
        , m_flags(flags)
    {}

    // -----------------------------------------------------------------
    // Template specializations for string
    // -----------------------------------------------------------------
    template<>
    std::string ConsoleVariable<std::string>::GetString() const {
        return m_value;
    }

    template<>
    void ConsoleVariable<std::string>::SetString(const std::string& value) {
        SetValue(value);
    }

    // -----------------------------------------------------------------
    // Explicit template instantiations for common types
    // -----------------------------------------------------------------
    template class ConsoleVariable<int>;
    template class ConsoleVariable<float>;
    template class ConsoleVariable<bool>;
    template class ConsoleVariable<std::string>;

} // namespace USE