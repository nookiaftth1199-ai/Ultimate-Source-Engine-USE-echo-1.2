// ============================================================
// FBXWrapper.h
// ============================================================
#pragma once
#include <fbxsdk.h>

namespace USE {
    class FBXWrapper {
    public:
        static bool Initialize();
        static void Shutdown();
        static FbxManager* GetManager() { return s_manager; }
        static FbxIOSettings* GetIOSettings() { return s_ioSettings; }
    private:
        static FbxManager* s_manager;
        static FbxIOSettings* s_ioSettings;
    };
}