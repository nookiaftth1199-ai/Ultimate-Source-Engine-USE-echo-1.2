// ============================================================
// Ultimate Source Engine - Core Platform Utilities
// ============================================================

#include "stdafx.h"
#include "Platform.h"
#include <thread>
#include <chrono>
#include <SDL.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/sysctl.h>
#endif

namespace USE
{
	int Platform::GetCPUCount()
	{
		return static_cast<int>(std::thread::hardware_concurrency());
	}

	std::string Platform::GetOSName()
	{
#ifdef _WIN32
		return "Windows";
#elif defined(__linux__)
		return "Linux";
#elif defined(__APPLE__)
		return "macOS";
#else
		return "Unknown";
#endif
	}

	uint64_t Platform::GetTimeMS()
	{
		return SDL_GetTicks();
	}

	uint32_t Platform::GetTotalRAMMB()
	{
#ifdef _WIN32
		MEMORYSTATUSEX mem;
		mem.dwLength = sizeof(mem);
		GlobalMemoryStatusEx(&mem);
		return static_cast<uint32_t>(mem.ullTotalPhys / (1024 * 1024));
#elif defined(__linux__)
		struct sysinfo info;
		if (sysinfo(&info) == 0)
			return static_cast<uint32_t>(info.totalram / (1024 * 1024));
		return 0;
#elif defined(__APPLE__)
		int64_t ram;
		size_t len = sizeof(ram);
		if (sysctlbyname("hw.memsize", &ram, &len, nullptr, 0) == 0)
			return static_cast<uint32_t>(ram / (1024 * 1024));
		return 0;
#else
		return 0;
#endif
	}

	void Platform::SleepMS(uint32_t milliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}
}