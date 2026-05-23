// ============================================================
// Ultimate Source Engine - File System Implementation
// ============================================================

#include "stdafx.h"
#include "FileSystem.h"
#include "Platform.h"
#include "Logger.h"

#include <fstream>
#include <sys/stat.h>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <fileapi.h>
    #include <shlobj.h>  // for known folders
    #pragma comment(lib, "shell32.lib")
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <errno.h>
#endif

namespace USE {

    // -----------------------------------------------------------------
    // File implementation
    // -----------------------------------------------------------------

    File::File() : m_cachedSize(0), m_mode(0) {}

    File::~File() { Close(); }

    bool File::Open(const std::string& path, int mode)
    {
        if (IsOpen()) Close();

        std::ios_base::openmode stdMode = std::ios_base::openmode();

        if (mode & FILE_BINARY) stdMode |= std::ios::binary;
        if (mode & FILE_AT_END) stdMode |= std::ios::ate;

        if ((mode & FILE_READ) && (mode & FILE_WRITE)) {
            stdMode |= std::ios::in | std::ios::out;
        } else if (mode & FILE_READ) {
            stdMode |= std::ios::in;
        } else if (mode & FILE_WRITE) {
            stdMode |= std::ios::out;
            if (mode & FILE_APPEND) stdMode |= std::ios::app;
            if (mode & FILE_TRUNCATE) stdMode |= std::ios::trunc;
        }

        m_stream.open(path.c_str(), stdMode);
        if (!m_stream.is_open()) {
            return false;
        }

        m_path = path;
        m_mode = mode;
        m_cachedSize = 0; // will be lazily computed

        return true;
    }

    bool File::OpenRead(const std::string& path)
    {
        return Open(path, FILE_READ);
    }

    bool File::OpenWrite(const std::string& path, bool append)
    {
        int mode = FILE_WRITE;
        if (append) mode |= FILE_APPEND;
        else mode |= FILE_TRUNCATE;
        return Open(path, mode);
    }

    void File::Close()
    {
        if (m_stream.is_open()) {
            m_stream.close();
        }
        m_path.clear();
        m_cachedSize = 0;
    }

    size_t File::Read(void* buffer, size_t elementSize, size_t count)
    {
        if (!IsOpen() || !(m_mode & FILE_READ)) return 0;
        m_stream.read(static_cast<char*>(buffer), elementSize * count);
        return static_cast<size_t>(m_stream.gcount()) / elementSize;
    }

    size_t File::Write(const void* buffer, size_t elementSize, size_t count)
    {
        if (!IsOpen() || !(m_mode & FILE_WRITE)) return 0;
        size_t bytes = elementSize * count;
        m_stream.write(static_cast<const char*>(buffer), bytes);
        return m_stream.good() ? count : 0;
    }

    bool File::Seek(int64_t offset, SeekOrigin origin)
    {
        if (!IsOpen()) return false;
        m_stream.seekg(offset, static_cast<std::ios_base::seekdir>(origin));
        m_stream.seekp(offset, static_cast<std::ios_base::seekdir>(origin));
        return m_stream.good();
    }

    int64_t File::Tell() const
    {
        if (!IsOpen()) return -1;
        return static_cast<int64_t>(m_stream.tellg());
    }

    bool File::Flush()
    {
        if (!IsOpen()) return false;
        m_stream.flush();
        return m_stream.good();
    }

    size_t File::GetSize() const
    {
        if (!IsOpen()) return 0;
        if (m_cachedSize > 0) return m_cachedSize;

        auto current = m_stream.tellg();
        m_stream.seekg(0, std::ios::end);
        m_cachedSize = static_cast<size_t>(m_stream.tellg());
        m_stream.seekg(current);
        return m_cachedSize;
    }

    // -----------------------------------------------------------------
    // FileSystem implementation
    // -----------------------------------------------------------------

    FileSystem::FileSystem() : m_initialized(false) {}

    FileSystem::~FileSystem()
    {
        Shutdown();
    }

    bool FileSystem::Initialize(const std::string& rootPath)
    {
        if (m_initialized) return true;

        USE_LOG_INFO("Initializing FileSystem...");

        // Determine application directory
        std::string exePath = Platform::GetExecutablePath();
        m_appDir = GetDirectoryName(exePath);

        // Add default search path: application directory
        AddSearchPath(m_appDir);

        // Optionally add working directory
        std::string workDir = Platform::GetWorkingDirectory();
        if (!workDir.empty() && workDir != m_appDir) {
            AddSearchPath(workDir);
        }

        m_initialized = true;
        USE_LOG_INFO("FileSystem initialized. AppDir: %s", m_appDir.c_str());
        return true;
    }

    void FileSystem::Shutdown()
    {
        if (!m_initialized) return;
        m_searchPaths.clear();
        m_initialized = false;
        USE_LOG_INFO("FileSystem shutdown.");
    }

    void FileSystem::AddSearchPath(const std::string& path, bool prepend)
    {
        if (path.empty()) return;
        std::string normalized = NormalizePath(path);
        // Ensure it ends with a separator
        if (!normalized.empty() && normalized.back() != '/' && normalized.back() != '\\') {
            normalized += '/';
        }
        if (prepend) {
            m_searchPaths.insert(m_searchPaths.begin(), normalized);
        } else {
            m_searchPaths.push_back(normalized);
        }
        USE_LOG_INFO("Added search path: %s", normalized.c_str());
    }

    void FileSystem::RemoveSearchPath(const std::string& path)
    {
        std::string normalized = NormalizePath(path);
        if (!normalized.empty() && normalized.back() != '/' && normalized.back() != '\\') {
            normalized += '/';
        }
        auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), normalized);
        if (it != m_searchPaths.end()) {
            m_searchPaths.erase(it);
            USE_LOG_INFO("Removed search path: %s", normalized.c_str());
        }
    }

    void FileSystem::ClearSearchPaths()
    {
        m_searchPaths.clear();
        USE_LOG_INFO("Cleared all search paths.");
    }

    std::string FileSystem::ResolvePath(const std::string& virtualPath) const
    {
        if (!m_initialized) return "";

        // If absolute path exists, return it
        std::string absPath = GetAbsolutePath(virtualPath);
        if (InternalFileExists(absPath)) {
            return absPath;
        }

        // If path is absolute but doesn't exist, return empty
        if (IsAbsolutePath(virtualPath)) {
            return "";
        }

        // Search in search paths
        for (const auto& base : m_searchPaths) {
            std::string combined = CombinePath(base, virtualPath);
            std::string native = NormalizePath(combined);
            if (InternalFileExists(native)) {
                return native;
            }
        }

        return "";
    }

    std::string FileSystem::FindFile(const std::string& virtualPath) const
    {
        return ResolvePath(virtualPath);
    }

    bool FileSystem::FileExists(const std::string& path) const
    {
        std::string resolved = ResolvePath(path);
        if (!resolved.empty()) {
            return true;
        }
        // If path is absolute and exists but not in search paths, still true
        if (IsAbsolutePath(path)) {
            return InternalFileExists(NormalizePath(path));
        }
        return false;
    }

    bool FileSystem::DirectoryExists(const std::string& path) const
    {
        std::string normalized = NormalizePath(path);
        // Ensure it ends with separator for check?
#ifdef _WIN32
        DWORD attr = GetFileAttributesA(normalized.c_str());
        return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat st;
        if (stat(normalized.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            return true;
        }
        return false;
#endif
    }

    bool FileSystem::CreateDirectory(const std::string& path)
    {
        std::string normalized = NormalizePath(path);
        if (DirectoryExists(normalized)) return true;

        // Create parent directories first
        size_t pos = normalized.find_first_of("/\\");
        while (pos != std::string::npos) {
            std::string parent = normalized.substr(0, pos);
            if (!parent.empty() && !DirectoryExists(parent)) {
                if (!InternalCreateDirectory(parent)) {
                    return false;
                }
            }
            pos = normalized.find_first_of("/\\", pos + 1);
        }
        return InternalCreateDirectory(normalized);
    }

    bool FileSystem::DeleteFile(const std::string& path)
    {
        std::string resolved = ResolvePath(path);
        if (resolved.empty()) resolved = NormalizePath(path);
#ifdef _WIN32
        return DeleteFileA(resolved.c_str()) != 0;
#else
        return unlink(resolved.c_str()) == 0;
#endif
    }

    bool FileSystem::GetFileInfo(const std::string& path, FileInfo& info) const
    {
        std::string resolved = ResolvePath(path);
        if (resolved.empty()) resolved = NormalizePath(path);
        return InternalGetFileInfo(resolved, info);
    }

    bool FileSystem::ListDirectory(const std::string& path, std::vector<FileInfo>& files, bool recursive) const
    {
        std::string resolved = ResolvePath(path);
        if (resolved.empty()) {
            // If path is absolute but not found via search, try directly
            if (IsAbsolutePath(path)) {
                resolved = NormalizePath(path);
            } else {
                return false;
            }
        }
        return InternalListDirectory(resolved, files, recursive);
    }

    std::string FileSystem::GetAbsolutePath(const std::string& path) const
    {
        if (path.empty()) return "";
        std::string normalized = NormalizePath(path);
        if (IsAbsolutePath(normalized)) {
            return normalized;
        }
        // Combine with current working directory
        std::string cwd = Platform::GetWorkingDirectory();
        return CombinePath(cwd, normalized);
    }

    std::string FileSystem::GetUserDirectory() const
    {
#ifdef _WIN32
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path))) {
            return std::string(path) + "\\My Games\\UltimateSourceEngine\\";
        }
#else
        const char* home = getenv("HOME");
        if (home) {
            return std::string(home) + "/.ultimatesourceengine/";
        }
#endif
        return m_appDir + "user/";
    }

    std::string FileSystem::GetTempDirectory() const
    {
#ifdef _WIN32
        char path[MAX_PATH];
        if (GetTempPathA(MAX_PATH, path)) {
            return std::string(path);
        }
#else
        const char* tmp = getenv("TMPDIR");
        if (tmp) return tmp;
        tmp = getenv("TEMP");
        if (tmp) return tmp;
        return "/tmp/";
#endif
        return "";
    }

    std::shared_ptr<File> FileSystem::OpenFile(const std::string& path, int mode)
    {
        std::string resolved;
        if (mode & FILE_WRITE) {
            // For writing, we need a writable location. Use search path only if exists, else create in app dir.
            resolved = ResolvePath(path);
            if (resolved.empty()) {
                // If not found, use first search path or app dir
                if (!m_searchPaths.empty()) {
                    resolved = CombinePath(m_searchPaths[0], path);
                } else {
                    resolved = CombinePath(m_appDir, path);
                }
                // Ensure directory exists
                std::string dir = GetDirectoryName(resolved);
                if (!dir.empty()) CreateDirectory(dir);
            }
        } else {
            resolved = ResolvePath(path);
            if (resolved.empty()) {
                return nullptr;
            }
        }

        std::shared_ptr<File> file = std::make_shared<File>();
        if (!file->Open(resolved, mode)) {
            return nullptr;
        }
        return file;
    }

    // -----------------------------------------------------------------
    // Static utility functions
    // -----------------------------------------------------------------

    std::string FileSystem::CombinePath(const std::string& path1, const std::string& path2)
    {
        if (path1.empty()) return path2;
        if (path2.empty()) return path1;

        char lastChar = path1.back();
        if (lastChar != '/' && lastChar != '\\') {
            return path1 + "/" + path2;
        } else {
            return path1 + path2;
        }
    }

    std::string FileSystem::GetFileName(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }

    std::string FileSystem::GetDirectoryName(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos) {
            return path.substr(0, pos);
        }
        return "";
    }

    std::string FileSystem::GetExtension(const std::string& path)
    {
        std::string name = GetFileName(path);
        size_t pos = name.find_last_of('.');
        if (pos != std::string::npos) {
            return name.substr(pos);
        }
        return "";
    }

    std::string FileSystem::ChangeExtension(const std::string& path, const std::string& newExt)
    {
        size_t pos = path.find_last_of('.');
        if (pos != std::string::npos) {
            return path.substr(0, pos) + newExt;
        }
        return path + newExt;
    }

    std::string FileSystem::NormalizePath(const std::string& path)
    {
        if (path.empty()) return path;

        std::string result;
        result.reserve(path.size());

        // Replace backslashes with forward slashes for consistency
        for (char c : path) {
            if (c == '\\') {
                result += '/';
            } else {
                result += c;
            }
        }

        // Remove redundant slashes
        std::string final;
        bool lastWasSlash = false;
        for (char c : result) {
            if (c == '/') {
                if (!lastWasSlash) {
                    final += c;
                    lastWasSlash = true;
                }
            } else {
                final += c;
                lastWasSlash = false;
            }
        }

        // Remove trailing slash unless it's root (e.g., "C:/")
        if (final.size() > 1 && final.back() == '/') {
            // Check if it's root like "C:/" or "/"
            if (final.size() == 3 && final[1] == ':') {
                // keep "C:/"
            } else if (final.size() == 1) {
                // keep "/"
            } else {
                final.pop_back();
            }
        }

        return final;
    }

    bool FileSystem::IsAbsolutePath(const std::string& path)
    {
        if (path.empty()) return false;
#ifdef _WIN32
        // Check for drive letter like C:\ or C:/
        if (path.size() >= 2 && std::isalpha(path[0]) && path[1] == ':') {
            return true;
        }
        // UNC paths?
        if (path.size() >= 2 && path[0] == '\\' && path[1] == '\\') {
            return true;
        }
        // Also consider / or \ at start
        return (path[0] == '/' || path[0] == '\\');
#else
        return path[0] == '/';
#endif
    }

    // -----------------------------------------------------------------
    // Platform-specific internal helpers
    // -----------------------------------------------------------------

    bool FileSystem::InternalFileExists(const std::string& nativePath) const
    {
#ifdef _WIN32
        DWORD attr = GetFileAttributesA(nativePath.c_str());
        return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat st;
        return (stat(nativePath.c_str(), &st) == 0 && S_ISREG(st.st_mode));
#endif
    }

    bool FileSystem::InternalGetFileInfo(const std::string& nativePath, FileInfo& info) const
    {
#ifdef _WIN32
        WIN32_FILE_ATTRIBUTE_DATA attrs;
        if (!GetFileAttributesExA(nativePath.c_str(), GetFileExInfoStandard, &attrs)) {
            return false;
        }
        info.name = GetFileName(nativePath);
        info.fullPath = nativePath;
        info.size = (static_cast<uint64_t>(attrs.nFileSizeHigh) << 32) | attrs.nFileSizeLow;
        info.creationTime = 0; // Convert from FILETIME if needed
        info.modificationTime = 0;
        info.isDirectory = (attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        info.isReadOnly = (attrs.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        return true;
#else
        struct stat st;
        if (stat(nativePath.c_str(), &st) != 0) return false;
        info.name = GetFileName(nativePath);
        info.fullPath = nativePath;
        info.size = static_cast<uint64_t>(st.st_size);
        info.creationTime = st.st_ctime;
        info.modificationTime = st.st_mtime;
        info.isDirectory = S_ISDIR(st.st_mode);
        info.isReadOnly = (st.st_mode & S_IWUSR) == 0;
        return true;
#endif
    }

    bool FileSystem::InternalCreateDirectory(const std::string& nativePath)
    {
#ifdef _WIN32
        return _mkdir(nativePath.c_str()) == 0;
#else
        return mkdir(nativePath.c_str(), 0755) == 0;
#endif
    }

    bool FileSystem::InternalListDirectory(const std::string& nativePath, std::vector<FileInfo>& files, bool recursive) const
    {
#ifdef _WIN32
        std::string searchPath = nativePath + "\\*";
        WIN32_FIND_DATA ffd;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE) {
            return false;
        }

        do {
            if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) continue;

            FileInfo info;
            info.name = ffd.cFileName;
            info.fullPath = CombinePath(nativePath, info.name);
            info.size = (static_cast<uint64_t>(ffd.nFileSizeHigh) << 32) | ffd.nFileSizeLow;
            info.isDirectory = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            info.isReadOnly = (ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
            files.push_back(info);

            if (recursive && info.isDirectory) {
                InternalListDirectory(info.fullPath, files, true);
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        FindClose(hFind);
        return true;
#else
        DIR* dir = opendir(nativePath.c_str());
        if (!dir) return false;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            std::string full = CombinePath(nativePath, entry->d_name);
            struct stat st;
            if (stat(full.c_str(), &st) != 0) continue;

            FileInfo info;
            info.name = entry->d_name;
            info.fullPath = full;
            info.size = static_cast<uint64_t>(st.st_size);
            info.creationTime = st.st_ctime;
            info.modificationTime = st.st_mtime;
            info.isDirectory = S_ISDIR(st.st_mode);
            info.isReadOnly = (st.st_mode & S_IWUSR) == 0;
            files.push_back(info);

            if (recursive && info.isDirectory) {
                InternalListDirectory(full, files, true);
            }
        }
        closedir(dir);
        return true;
#endif
    }

} // namespace USE