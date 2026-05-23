// ============================================================
// FBXWrapper.cpp
// ============================================================
#include "FBXWrapper.h"
#include "Core/Logger.h"

namespace USE {
    FbxManager* FBXWrapper::s_manager = nullptr;
    FbxIOSettings* FBXWrapper::s_ioSettings = nullptr;

    bool FBXWrapper::Initialize() {
        s_manager = FbxManager::Create();
        if (!s_manager) {
            USE_LOG_ERROR("FBX SDK: Failed to create manager");
            return false;
        }
        s_ioSettings = FbxIOSettings::Create(s_manager, IOSROOT);
        s_manager->SetIOSettings(s_ioSettings);
        USE_LOG_INFO("FBX SDK initialized");
        return true;
    }

    void FBXWrapper::Shutdown() {
        if (s_manager) {
            s_manager->Destroy();
            s_manager = nullptr;
            s_ioSettings = nullptr;
        }
        USE_LOG_INFO("FBX SDK shut down");
    }
}