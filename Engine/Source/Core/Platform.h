// ============================================================
// Ultimate Source Engine - Core Platform Utilities
// ============================================================
// Basic system information and CPU detection.
// ============================================================

#pragma once

#include <cstdint>
#include <string>

namespace USE
{
	class Platform
	{
	public:
		// Get the number of logical CPU cores.
		static int GetCPUCount();

		// Get a human‑readable operating system name.
		static std::string GetOSName();

		// Get the current system time in milliseconds.
		static uint64_t GetTimeMS();

		// Get the amount of installed RAM in megabytes.
		static uint32_t GetTotalRAMMB();

		// Sleep the current thread for a specified number of milliseconds.
		static void SleepMS(uint32_t milliseconds);
	};
}