// ============================================================
// Ultimate Source Engine - String Utilities Implementation
// ============================================================

#include "stdafx.h"
#include "StringUtils.h"
#include <cctype>
#include <sstream>
#include <cstdarg>

namespace USE {
namespace StringUtils {

    std::string Trim(const std::string& str) {
        return TrimLeft(TrimRight(str));
    }

    std::string TrimLeft(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        return (start == std::string::npos) ? "" : str.substr(start);
    }

    std::string TrimRight(const std::string& str) {
        size_t end = str.find_last_not_of(" \t\n\r");
        return (end == std::string::npos) ? "" : str.substr(0, end + 1);
    }

    std::string ToLower(const std::string& str) {
        std::string result = str;
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
        }
        return result;
    }

    std::string ToUpper(const std::string& str) {
        std::string result = str;
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
        }
        return result;
    }

    std::vector<std::string> Split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }

    std::string Join(const std::vector<std::string>& parts, const std::string& separator) {
        std::string result;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i != 0) result += separator;
            result += parts[i];
        }
        return result;
    }

    std::string Replace(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    bool StartsWith(const std::string& str, const std::string& prefix) {
        if (prefix.length() > str.length()) return false;
        return str.compare(0, prefix.length(), prefix) == 0;
    }

    bool EndsWith(const std::string& str, const std::string& suffix) {
        if (suffix.length() > str.length()) return false;
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    int ToInt(const std::string& str) {
        std::stringstream ss(str);
        int value = 0;
        ss >> value;
        return value;
    }

    float ToFloat(const std::string& str) {
        std::stringstream ss(str);
        float value = 0.0f;
        ss >> value;
        return value;
    }

    bool ToBool(const std::string& str) {
        std::string lower = ToLower(Trim(str));
        return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
    }

    std::string FromInt(int value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::string FromFloat(float value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::string FromBool(bool value) {
        return value ? "true" : "false";
    }

    std::string Format(const char* format, ...) {
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        return std::string(buffer);
    }

} // namespace StringUtils
} // namespace USE