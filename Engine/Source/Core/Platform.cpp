// ============================================================
// Ultimate Source Engine - Platform Abstraction Implementation
// ============================================================

#include "stdafx.h"
#include "Platform.h"

#if USE_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <psapi.h>
    #include <shellapi.h>
    #include <intrin.h>
    #pragma comment(lib, "shell32.lib")
#elif USE_PLATFORM_LINUX
    #include <unistd.h>
    #include <sys/sysinfo.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <pthread.h>
    #include <dlfcn.h>
    #include <limits.h>
    #include <errno.h>
#elif USE_PLATFORM_MACOS
    #include <unistd.h>
    #include <sys/sysctl.h>
    #include <sys/types.h>
    #include <mach/mach.h>
    #include <mach/thread_act.h>
    #include <pthread.h>
    #include <dlfcn.h>
    #include <libproc.h>
    #include <errno.h>
#endif

#include <thread>
#include <chrono>

namespace USE {
namespace Platform {

    // -----------------------------------------------------------------
    // System Information
    // -----------------------------------------------------------------

    const char* GetPlatformName()
    {
        return USE_PLATFORM_NAME;
    }

    const char* GetArchitectureName()
    {
        return USE_ARCH_NAME;
    }

    const char* GetCompilerName()
    {
        return USE_COMPILER_NAME;
    }

    // -----------------------------------------------------------------
    // CPU Count
    // -----------------------------------------------------------------

    int GetCPUCount()
    {
        static int cached = 0;
        if (cached != 0) return cached;

        #if USE_PLATFORM_WINDOWS
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            cached = sysInfo.dwNumberOfProcessors;
        #elif USE_PLATFORM_LINUX
            cached = sysconf(_SC_NPROCESSORS_ONLN);
        #elif USE_PLATFORM_MACOS
            int nm[2];
            size_t len = 4;
            uint32_t count;
            nm[0] = CTL_HW;
            nm[1] = HW_AVAILCPU;
            sysctl(nm, 2, &count, &len, NULL, 0);
            if (count < 1) {
                nm[1] = HW_NCPU;
                sysctl(nm, 2, &count, &len, NULL, 0);
                if (count < 1) count = 1;
            }
            cached = count;
        #endif

        if (cached < 1) cached = 1;
        return cached;
    }

    // -----------------------------------------------------------------
    // Process ID
    // -----------------------------------------------------------------

    uint32 GetProcessID()
    {
        #if USE_PLATFORM_WINDOWS
            return GetCurrentProcessId();
        #else
            return static_cast<uint32>(getpid());
        #endif
    }

    // -----------------------------------------------------------------
    // Thread ID
    // -----------------------------------------------------------------

    uint32 GetThreadID()
    {
        #if USE_PLATFORM_WINDOWS
            return GetCurrentThreadId();
        #elif USE_PLATFORM_LINUX || USE_PLATFORM_MACOS
            return static_cast<uint32>(pthread_self());
        #endif
    }

    // -----------------------------------------------------------------
    // Executable Path
    // -----------------------------------------------------------------

    std::string GetExecutablePath()
    {
        std::string path;
        #if USE_PLATFORM_WINDOWS
            char buffer[MAX_PATH];
            GetModuleFileNameA(NULL, buffer, MAX_PATH);
            path = buffer;
        #elif USE_PLATFORM_LINUX
            char buffer[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
            if (len != -1) {
                buffer[len] = '\0';
                path = buffer;
            }
        #elif USE_PLATFORM_MACOS
            char buffer[PROC_PIDPATHINFO_MAXSIZE];
            pid_t pid = getpid();
            int ret = proc_pidpath(pid, buffer, sizeof(buffer));
            if (ret > 0) {
                path = buffer;
            }
        #endif
        return path;
    }

    // -----------------------------------------------------------------
    // Working Directory
    // -----------------------------------------------------------------

    std::string GetWorkingDirectory()
    {
        std::string path;
        #if USE_PLATFORM_WINDOWS
            char buffer[MAX_PATH];
            if (GetCurrentDirectoryA(MAX_PATH, buffer)) {
                path = buffer;
            }
        #else
            char buffer[PATH_MAX];
            if (getcwd(buffer, sizeof(buffer))) {
                path = buffer;
            }
        #endif
        return path;
    }

    bool SetWorkingDirectory(const std::string& path)
    {
        #if USE_PLATFORM_WINDOWS
            return SetCurrentDirectoryA(path.c_str()) != 0;
        #else
            return chdir(path.c_str()) == 0;
        #endif
    }

    // -----------------------------------------------------------------
    // Environment Variables
    // -----------------------------------------------------------------

    std::string GetEnvironmentVariable(const std::string& name)
    {
        #if USE_PLATFORM_WINDOWS
            char buffer[32768];
            DWORD len = GetEnvironmentVariableA(name.c_str(), buffer, sizeof(buffer));
            if (len > 0 && len < sizeof(buffer)) {
                return std::string(buffer, len);
            }
        #else
            const char* value = getenv(name.c_str());
            if (value) {
                return value;
            }
        #endif
        return "";
    }

    bool SetEnvironmentVariable(const std::string& name, const std::string& value)
    {
        #if USE_PLATFORM_WINDOWS
            return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0;
        #else
            return setenv(name.c_str(), value.c_str(), 1) == 0;
        #endif
    }

    // -----------------------------------------------------------------
    // High-Performance Timer
    // -----------------------------------------------------------------

    uint64 GetMicroseconds()
    {
        static uint64 freq = 0;
        #if USE_PLATFORM_WINDOWS
            LARGE_INTEGER counter;
            if (freq == 0) {
                LARGE_INTEGER f;
                QueryPerformanceFrequency(&f);
                freq = f.QuadPart;
            }
            QueryPerformanceCounter(&counter);
            return counter.QuadPart * 1000000 / freq;
        #else
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        #endif
    }

    // -----------------------------------------------------------------
    // Memory Page Size
    // -----------------------------------------------------------------

    size_t GetPageSize()
    {
        #if USE_PLATFORM_WINDOWS
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            return sysInfo.dwPageSize;
        #else
            return static_cast<size_t>(sysconf(_SC_PAGESIZE));
        #endif
    }

    // -----------------------------------------------------------------
    // Dynamic Library Loading
    // -----------------------------------------------------------------

    Handle LoadLibrary(const std::string& path)
    {
        #if USE_PLATFORM_WINDOWS
            return static_cast<Handle>(LoadLibraryA(path.c_str()));
        #else
            return static_cast<Handle>(dlopen(path.c_str(), RTLD_LAZY));
        #endif
    }

    void* GetLibrarySymbol(Handle lib, const std::string& symbol)
    {
        #if USE_PLATFORM_WINDOWS
            return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(lib), symbol.c_str()));
        #else
            return dlsym(lib, symbol.c_str());
        #endif
    }

    void UnloadLibrary(Handle lib)
    {
        #if USE_PLATFORM_WINDOWS
            FreeLibrary(static_cast<HMODULE>(lib));
        #else
            dlclose(lib);
        #endif
    }

    // -----------------------------------------------------------------
    // Last Error String
    // -----------------------------------------------------------------

    std::string GetLastErrorString()
    {
        #if USE_PLATFORM_WINDOWS
            DWORD error = GetLastError();
            if (error == 0) return "";
            char* buffer = nullptr;
            DWORD len = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<char*>(&buffer), 0, nullptr);
            if (len > 0) {
                std::string result(buffer, len);
                LocalFree(buffer);
                return result;
            }
        #else
            int error = errno;
            if (error != 0) {
                return strerror(error);
            }
        #endif
        return "";
    }

    // -----------------------------------------------------------------
    // Debug Output
    // -----------------------------------------------------------------

    void DebugOutput(const char* message)
    {
        #if USE_PLATFORM_WINDOWS
            OutputDebugStringA(message);
            OutputDebugStringA("\n");
        #else
            fprintf(stderr, "%s\n", message);
            fflush(stderr);
        #endif
    }

    // -----------------------------------------------------------------
    // Message Box
    // -----------------------------------------------------------------

    void ShowMessageBox(const char* title, const char* message, bool isError)
    {
        #if USE_PLATFORM_WINDOWS
            UINT type = MB_OK;
            if (isError) type |= MB_ICONERROR;
            else type |= MB_ICONINFORMATION;
            MessageBoxA(nullptr, message, title, type);
        #else
            fprintf(isError ? stderr : stdout, "%s: %s\n", title, message);
            fflush(isError ? stderr : stdout);
            // On non-Windows, we could use X11 or Cocoa dialogs, but for simplicity we just print.
        #endif
    }

} // namespace Platform
} // namespace USE