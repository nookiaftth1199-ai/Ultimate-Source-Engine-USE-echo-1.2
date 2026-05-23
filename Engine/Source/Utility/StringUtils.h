// ============================================================
// Ultimate Source Engine - String Utilities
// ============================================================
//
// Collection of helper functions for string manipulation.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

namespace USE {
namespace StringUtils {

    // Trim whitespace from left and right.
    std::string Trim(const std::string& str);
    std::string TrimLeft(const std::string& str);
    std::string TrimRight(const std::string& str);

    // Convert to lower/upper case.
    std::string ToLower(const std::string& str);
    std::string ToUpper(const std::string& str);

    // Split string by delimiter.
    std::vector<std::string> Split(const std::string& str, char delimiter);

    // Join vector of strings with separator.
    std::string Join(const std::vector<std::string>& parts, const std::string& separator);

    // Replace all occurrences of 'from' with 'to'.
    std::string Replace(const std::string& str, const std::string& from, const std::string& to);

    // Check if string starts/ends with a substring.
    bool StartsWith(const std::string& str, const std::string& prefix);
    bool EndsWith(const std::string& str, const std::string& suffix);

    // Convert between types.
    int         ToInt(const std::string& str);
    float       ToFloat(const std::string& str);
    bool        ToBool(const std::string& str);
    std::string FromInt(int value);
    std::string FromFloat(float value);
    std::string FromBool(bool value);

    // Format string (like sprintf but returns std::string).
    std::string Format(const char* format, ...);

} // namespace StringUtils
} // namespace USE