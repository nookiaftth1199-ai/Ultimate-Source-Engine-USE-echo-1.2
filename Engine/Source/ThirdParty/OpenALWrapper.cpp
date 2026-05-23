// ============================================================
// OpenALWrapper.cpp
// ============================================================
#include "OpenALWrapper.h"
#include "Core/Logger.h"

namespace USE {
    ALCdevice* OpenALWrapper::s_device = nullptr;
    ALCcontext* OpenALWrapper::s_context = nullptr;

    bool OpenALWrapper::Initialize() {
        s_device = alcOpenDevice(nullptr);
        if (!s_device) {
            USE_LOG_ERROR("OpenAL: Could not open device");
            return false;
        }
        s_context = alcCreateContext(s_device, nullptr);
        if (!s_context) {
            USE_LOG_ERROR("OpenAL: Could not create context");
            alcCloseDevice(s_device);
            s_device = nullptr;
            return false;
        }
        alcMakeContextCurrent(s_context);
        USE_LOG_INFO("OpenAL initialized");
        return true;
    }

    void OpenALWrapper::Shutdown() {
        if (s_context) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(s_context);
            s_context = nullptr;
        }
        if (s_device) {
            alcCloseDevice(s_device);
            s_device = nullptr;
        }
        USE_LOG_INFO("OpenAL shut down");
    }
}