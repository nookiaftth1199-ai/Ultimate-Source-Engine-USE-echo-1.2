#pragma once

#define USE_ENGINE_NAME      "Ultimate Source Engine"
#define USE_ENGINE_VERSION   "1.0.0"
#define USE_ENGINE_CODENAME  "Genesis"
#define USE_ENGINE_BUILD     1001

#define USE_COPYRIGHT_YEAR   "2024"
#define USE_COMPANY_NAME     "Ultimate Source Interactive"

namespace USE {
    
    struct EngineVersion {
        static const char* GetFullString() {
            static char version[256];
            sprintf_s(version, "%s %s (%s Build %d)", 
                     USE_ENGINE_NAME, 
                     USE_ENGINE_VERSION, 
                     USE_ENGINE_CODENAME,
                     USE_ENGINE_BUILD);
            return version;
        }
        
        static int GetMajor() { return 1; }
        static int GetMinor() { return 0; }
        static int GetPatch() { return 0; }
        static int GetBuild() { return USE_ENGINE_BUILD; }
    };
    
} // namespace USE