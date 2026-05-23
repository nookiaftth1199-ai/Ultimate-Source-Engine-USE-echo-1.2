// ============================================================
// Ultimate Source Engine - Precompiled Header (VS2015+)
// ============================================================
#pragma once

// -----------------------------------------------------------------
// Target Windows version (Windows 7 or later)
// -----------------------------------------------------------------
#ifndef WINVER
#define WINVER 0x0601
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

// -----------------------------------------------------------------
// Windows headers
// -----------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>

// -----------------------------------------------------------------
// C and C++ standard library (full C++11/14 support)
// -----------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <type_traits>

// -----------------------------------------------------------------
// Engine macros and utilities
// -----------------------------------------------------------------
#define ENGINE_API __declspec(dllexport)
#define SAFE_DELETE(p)      { if(p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p){ if(p) { delete[] (p); (p) = nullptr; } }
#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p) = nullptr; } }

// -----------------------------------------------------------------
// Disable common MSVC warnings
// -----------------------------------------------------------------
#pragma warning(disable: 4996)  // deprecated functions (fopen, strcpy, etc.)
#pragma warning(disable: 4251)  // dll-interface class used as base
#pragma warning(disable: 4275)  // non dll-interface class used as base
#pragma warning(disable: 4100)  // unreferenced formal parameter
#pragma warning(disable: 4127)  // conditional expression is constant

// -----------------------------------------------------------------
// Optional: include math constants (not standard in VS2015, but we define our own)
// -----------------------------------------------------------------
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------------------------------
// Engine forward declarations (optional, included as needed in deeper headers)
// -----------------------------------------------------------------
namespace USE {
    class Application;
    class Engine;
    class Window;
    class SDLWindow;
    class Timer;
    class MemoryManager;
    class FileSystem;
    class ConfigManager;
    class EventSystem;
    class ThreadPool;
    class JobSystem;
    class RenderSystem;
    class IRenderDevice;
    class Shader;
    class Texture;
    class Material;
    class Mesh;
    class Model;
    class Camera;
    class Light;
    class Skybox;
    class DebugDraw;
    // ... other classes are included in their respective headers
}

// -----------------------------------------------------------------
// Core math types (commonly used)
// -----------------------------------------------------------------
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"