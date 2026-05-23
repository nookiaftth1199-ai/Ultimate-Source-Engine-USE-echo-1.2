// ============================================================
// Ultimate Source Engine - Target Version Definition
// ============================================================
//
// This file defines the minimum Windows version that the engine
// will support. It sets the WINVER and _WIN32_WINNT macros
// accordingly.
//
// Target: Windows 7 (0x0601) or later
// ============================================================

#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform,
// include WinSDKVer.h and set the _WIN32_WINNT macro to the platform you
// wish to support before including SDKDDKVer.h.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#ifndef WINVER                  // Specifies that the minimum required platform is Windows 7.
#define WINVER 0x0601           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows 7.
#define _WIN32_WINNT 0x0601     // Change this to the appropriate value to target other versions of Windows.
#endif

// Windows 2000 (0x0500)
// Windows XP  (0x0501)
// Windows Vista (0x0600)
// Windows 7 (0x0601)
// Windows 8 (0x0602)
// Windows 8.1 (0x0603)
// Windows 10 (0x0A00)

// Uncomment to target Windows XP:
//#define WINVER 0x0501
//#define _WIN32_WINNT 0x0501

// Uncomment to target Windows Vista:
//#define WINVER 0x0600
//#define _WIN32_WINNT 0x0600

#include <SDKDDKVer.h>
