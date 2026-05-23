// ============================================================
// Ultimate Source Engine - Precompiled Header (VS2010)
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
// C standard library
// -----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <stddef.h>
#include <crtdbg.h>         // for memory leak detection

// -----------------------------------------------------------------
// C++ STL (VS2010 supported subset)
// -----------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <memory>           // auto_ptr, shared_ptr (TR1)
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdint>          // works in VS2010
#include <functional>       // std::function (partial, but usable)
#include <utility>

// -----------------------------------------------------------------
// Engine macros
// -----------------------------------------------------------------
#define ENGINE_API __declspec(dllexport)
#define SAFE_DELETE(p)      { if(p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p){ if(p) { delete[] (p); (p) = nullptr; } }
#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p) = nullptr; } }

// -----------------------------------------------------------------
// Disable harmless warnings
// -----------------------------------------------------------------
#pragma warning(disable: 4996)  // deprecated functions (fopen, strcpy, etc.)
#pragma warning(disable: 4251)  // dll-interface class used as base
#pragma warning(disable: 4275)  // non dll-interface class used as base
#pragma warning(disable: 4100)  // unreferenced formal parameter
#pragma warning(disable: 4127)  // conditional expression is constant

// -----------------------------------------------------------------
// Engine forward declarations
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
    // ... other classes are included as needed in their respective headers
}

// -----------------------------------------------------------------
// Math types (often used everywhere)
// -----------------------------------------------------------------
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"