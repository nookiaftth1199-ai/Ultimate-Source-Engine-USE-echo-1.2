// ============================================================
// Ultimate Source Engine - File Utilities
// ============================================================
//
// Cross‑platform file and directory operations.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

namespace USE {
namespace FileUtils {

    // Check if a file exists.
    bool FileExists(const std::string& path);

    // Check if a directory exists.
    bool DirectoryExists(const std::string& path);

    // Create a directory (including intermediate directories if needed).
    bool CreateDirectory(const std::string& path);

    // Delete a file or empty directory.
    bool DeleteFile(const std::string& path);
    bool DeleteDirectory(const std::string& path);

    // Copy a file (overwrite if exists).
    bool CopyFile(const std::string& src, const std::string& dst, bool overwrite = true);

    // Move a file (rename).
    bool MoveFile(const std::string& src, const std::string& dst);

    // Get file size in bytes (returns -1 on error).
    int64_t GetFileSize(const std::string& path);

    // Get last modification time (seconds since epoch).
    time_t GetLastModifiedTime(const std::string& path);

    // Read entire text file into a string.
    bool ReadTextFile(const std::string& path, std::string& outContent);

    // Write a string to a text file (overwrites).
    bool WriteTextFile(const std::string& path, const std::string& content);

    // Read binary file into a byte buffer.
    bool ReadBinaryFile(const std::string& path, std::vector<uint8_t>& outData);

    // Write binary buffer to a file.
    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data);

    // List files in a directory (non‑recursive). Returns filenames (without path).
    std::vector<std::string> ListFiles(const std::string& directory, const std::string& filter = "");

    // Get current working directory.
    std::string GetCurrentDirectory();

    // Change current working directory.
    bool SetCurrentDirectory(const std::string& path);

    // Get the directory part of a path (e.g., "/foo/bar.txt" -> "/foo").
    std::string GetDirectory(const std::string& path);

    // Get the filename part of a path (e.g., "/foo/bar.txt" -> "bar.txt").
    std::string GetFileName(const std::string& path);

    // Get the extension (including dot, e.g., ".txt").
    std::string GetExtension(const std::string& path);

    // Change the extension of a path.
    std::string ChangeExtension(const std::string& path, const std::string& newExt);

    // Combine two path components (adds separator if needed).
    std::string CombinePath(const std::string& a, const std::string& b);

    // Normalize path (convert backslashes to slashes, remove redundant separators).
    std::string NormalizePath(const std::string& path);

} // namespace FileUtils
} // namespace USE