// ============================================================
// Ultimate Source Engine - File Utilities Implementation
// ============================================================

#include "stdafx.h"
#include "FileUtils.h"
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#define USE_MKDIR _mkdir
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cerrno>
#define USE_MKDIR(path) mkdir(path, 0755)
#endif

namespace USE {
namespace FileUtils {

    bool FileExists(const std::string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && !(buffer.st_mode & S_IFDIR);
    }

    bool DirectoryExists(const std::string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0) && (buffer.st_mode & S_IFDIR);
    }

    bool CreateDirectory(const std::string& path) {
        if (DirectoryExists(path)) return true;

        size_t pos = 0;
        std::string buildPath;
        while (true) {
            pos = path.find_first_of("/\\", pos + 1);
            buildPath = path.substr(0, pos);
            if (!buildPath.empty() && !DirectoryExists(buildPath)) {
                if (USE_MKDIR(buildPath.c_str()) != 0) {
                    return false;
                }
            }
            if (pos == std::string::npos) break;
        }
        return true;
    }

    bool DeleteFile(const std::string& path) {
#ifdef _WIN32
        return ::DeleteFileA(path.c_str()) != 0;
#else
        return unlink(path.c_str()) == 0;
#endif
    }

    bool DeleteDirectory(const std::string& path) {
        if (!DirectoryExists(path)) return false;
#ifdef _WIN32
        return ::RemoveDirectoryA(path.c_str()) != 0;
#else
        return rmdir(path.c_str()) == 0;
#endif
    }

    bool CopyFile(const std::string& src, const std::string& dst, bool overwrite) {
        if (!overwrite && FileExists(dst)) return false;
        std::ifstream srcStream(src, std::ios::binary);
        if (!srcStream.is_open()) return false;
        std::ofstream dstStream(dst, std::ios::binary | std::ios::trunc);
        if (!dstStream.is_open()) return false;
        dstStream << srcStream.rdbuf();
        return true;
    }

    bool MoveFile(const std::string& src, const std::string& dst) {
#ifdef _WIN32
        return ::MoveFileA(src.c_str(), dst.c_str()) != 0;
#else
        return rename(src.c_str(), dst.c_str()) == 0;
#endif
    }

    int64_t GetFileSize(const std::string& path) {
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return -1;
        return static_cast<int64_t>(st.st_size);
    }

    time_t GetLastModifiedTime(const std::string& path) {
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return 0;
        return st.st_mtime;
    }

    bool ReadTextFile(const std::string& path, std::string& outContent) {
        std::ifstream file(path);
        if (!file.is_open()) return false;
        std::stringstream ss;
        ss << file.rdbuf();
        outContent = ss.str();
        return true;
    }

    bool WriteTextFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::trunc);
        if (!file.is_open()) return false;
        file << content;
        return true;
    }

    bool ReadBinaryFile(const std::string& path, std::vector<uint8_t>& outData) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return false;
        file.seekg(0, std::ios::end);
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        outData.resize(size);
        file.read(reinterpret_cast<char*>(outData.data()), size);
        return true;
    }

    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data) {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) return false;
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }

    std::vector<std::string> ListFiles(const std::string& directory, const std::string& filter) {
        std::vector<std::string> result;
        DIR* dir = opendir(directory.c_str());
        if (!dir) return result;
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name == "." || name == "..") continue;
            if (!filter.empty() && name.find(filter) == std::string::npos) continue;
            result.push_back(name);
        }
        closedir(dir);
        return result;
    }

    std::string GetCurrentDirectory() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        if (::GetCurrentDirectoryA(MAX_PATH, buffer) == 0) return "";
        return std::string(buffer);
#else
        char buffer[PATH_MAX];
        if (getcwd(buffer, sizeof(buffer)) == nullptr) return "";
        return std::string(buffer);
#endif
    }

    bool SetCurrentDirectory(const std::string& path) {
#ifdef _WIN32
        return ::SetCurrentDirectoryA(path.c_str()) != 0;
#else
        return chdir(path.c_str()) == 0;
#endif
    }

    std::string GetDirectory(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return "";
        return path.substr(0, pos);
    }

    std::string GetFileName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return path;
        return path.substr(pos + 1);
    }

    std::string GetExtension(const std::string& path) {
        std::string name = GetFileName(path);
        size_t pos = name.find_last_of('.');
        if (pos == std::string::npos) return "";
        return name.substr(pos);
    }

    std::string ChangeExtension(const std::string& path, const std::string& newExt) {
        std::string base = path;
        size_t pos = base.find_last_of('.');
        if (pos != std::string::npos) base = base.substr(0, pos);
        return base + newExt;
    }

    std::string CombinePath(const std::string& a, const std::string& b) {
        if (a.empty()) return b;
        if (b.empty()) return a;
        char last = a.back();
        if (last == '/' || last == '\\')
            return a + b;
        else
            return a + "/" + b;
    }

    std::string NormalizePath(const std::string& path) {
        std::string result;
        result.reserve(path.size());
        for (char c : path) {
            if (c == '\\') result += '/';
            else result += c;
        }
        // Remove duplicate slashes
        std::string final;
        bool lastWasSlash = false;
        for (char c : result) {
            if (c == '/') {
                if (!lastWasSlash) final += c;
                lastWasSlash = true;
            } else {
                final += c;
                lastWasSlash = false;
            }
        }
        return final;
    }

} // namespace FileUtils
} // namespace USE