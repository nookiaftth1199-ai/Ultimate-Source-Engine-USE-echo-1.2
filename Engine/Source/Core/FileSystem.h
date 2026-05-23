// ============================================================
// Ultimate Source Engine - File System
// ============================================================
//
// Provides virtual file system abstraction with search paths,
// file I/O, directory enumeration, and archive support.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include <memory>

namespace USE {

    // -----------------------------------------------------------------
    // File open modes (compatible with std::ios_base::openmode)
    // -----------------------------------------------------------------
    enum FileMode {
        FILE_READ       = 1 << 0,
        FILE_WRITE      = 1 << 1,
        FILE_BINARY     = 1 << 2,
        FILE_APPEND     = 1 << 3,
        FILE_TRUNCATE   = 1 << 4,
        FILE_AT_END     = 1 << 5,
        FILE_TEXT       = 0,          // default, not binary
    };

    inline int operator|(int a, FileMode b) { return a | static_cast<int>(b); }
    inline int operator|(FileMode a, FileMode b) { return static_cast<int>(a) | static_cast<int>(b); }

    // -----------------------------------------------------------------
    // Seek origin
    // -----------------------------------------------------------------
    enum SeekOrigin {
        SEEK_BEGIN = std::ios::beg,
        SEEK_CUR   = std::ios::cur,
        SEEK_END   = std::ios::end,
    };

    // -----------------------------------------------------------------
    // File information structure
    // -----------------------------------------------------------------
    struct FileInfo {
        std::string name;         // file name (no path)
        std::string fullPath;      // full absolute path
        uint64_t    size;          // file size in bytes
        time_t      creationTime;   // creation time (platform dependent)
        time_t      modificationTime; // last write time
        bool        isDirectory;     // true if directory
        bool        isReadOnly;      // true if read-only
    };

    // -----------------------------------------------------------------
    // File handle wrapper (RAII)
    // -----------------------------------------------------------------
    class File {
    public:
        File();
        ~File();

        // Open a file (using virtual path resolution)
        bool Open(const std::string& path, int mode = FILE_READ);
        bool OpenWrite(const std::string& path, bool append = false);
        bool OpenRead(const std::string& path);

        // Close explicitly (also on destruct)
        void Close();

        // I/O
        size_t Read(void* buffer, size_t elementSize, size_t count);
        size_t Write(const void* buffer, size_t elementSize, size_t count);
        bool   Seek(int64_t offset, SeekOrigin origin = SEEK_BEGIN);
        int64_t Tell() const;
        bool   Flush();
        bool   IsOpen() const { return m_stream.is_open(); }
        bool   IsEOF() const { return m_stream.eof(); }
        size_t GetSize() const;   // returns file size (may cache)

        // Path used to open
        const std::string& GetPath() const { return m_path; }

        // Disable copy
        File(const File&) = delete;
        File& operator=(const File&) = delete;

    private:
        mutable std::fstream m_stream;
        std::string          m_path;
        mutable size_t       m_cachedSize; // cached file size
        int                  m_mode;
    };

    // -----------------------------------------------------------------
    // Main FileSystem class (accessed via Engine::GetFileSystem)
    // -----------------------------------------------------------------
    class FileSystem {
    public:
        FileSystem();
        ~FileSystem();

        // Initialize with root path (application directory)
        bool Initialize(const std::string& rootPath = "");
        void Shutdown();

        // Search paths
        void AddSearchPath(const std::string& path, bool prepend = false);
        void RemoveSearchPath(const std::string& path);
        void ClearSearchPaths();
        const std::vector<std::string>& GetSearchPaths() const { return m_searchPaths; }

        // Resolve a virtual path to an actual file system path.
        // Returns empty string if not found.
        std::string ResolvePath(const std::string& virtualPath) const;

        // Check if a file exists (in search paths or absolute)
        bool FileExists(const std::string& path) const;

        // Check if a directory exists
        bool DirectoryExists(const std::string& path) const;

        // Create directory (including parent directories)
        bool CreateDirectory(const std::string& path);

        // Delete file
        bool DeleteFile(const std::string& path);

        // Get file info
        bool GetFileInfo(const std::string& path, FileInfo& info) const;

        // List contents of a directory (relative to search paths or absolute)
        bool ListDirectory(const std::string& path, std::vector<FileInfo>& files, bool recursive = false) const;

        // Get the absolute path (resolve relative and normalize)
        std::string GetAbsolutePath(const std::string& path) const;

        // Get the application directory
        std::string GetApplicationDirectory() const { return m_appDir; }

        // Get the user write directory (for config, saves, logs)
        std::string GetUserDirectory() const;

        // Get the temp directory
        std::string GetTempDirectory() const;

        // Open a file using virtual path resolution
        std::shared_ptr<File> OpenFile(const std::string& path, int mode = FILE_READ);

        // Utility: combine paths
        static std::string CombinePath(const std::string& path1, const std::string& path2);
        static std::string GetFileName(const std::string& path);
        static std::string GetDirectoryName(const std::string& path);
        static std::string GetExtension(const std::string& path);
        static std::string ChangeExtension(const std::string& path, const std::string& newExt);
        static std::string NormalizePath(const std::string& path);
        static bool IsAbsolutePath(const std::string& path);

    private:
        std::vector<std::string> m_searchPaths;
        std::string              m_appDir;
        bool                     m_initialized;

        // Internal helper to find file in search paths
        std::string FindFile(const std::string& virtualPath) const;

        // Platform-specific helpers
        bool InternalFileExists(const std::string& nativePath) const;
        bool InternalGetFileInfo(const std::string& nativePath, FileInfo& info) const;
        bool InternalCreateDirectory(const std::string& nativePath);
        bool InternalListDirectory(const std::string& nativePath, std::vector<FileInfo>& files, bool recursive) const;
    };

} // namespace USE