// ============================================================
// OpenALWrapper.h
// ============================================================
#pragma once
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {
    class OpenALWrapper {
    public:
        static bool Initialize();
        static void Shutdown();
        static ALCdevice* GetDevice() { return s_device; }
        static ALCcontext* GetContext() { return s_context; }
    private:
        static ALCdevice* s_device;
        static ALCcontext* s_context;
    };
}