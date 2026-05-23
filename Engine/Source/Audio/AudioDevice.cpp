// ============================================================
// Ultimate Source Engine - Audio Device Implementation
// ============================================================

#include "stdafx.h"
#include "AudioDevice.h"
#include "Core/Logger.h"

// OpenAL headers
#include <AL/al.h>
#include <AL/alc.h>

namespace USE {

    AudioDevice::AudioDevice()
        : m_device(nullptr)
        , m_context(nullptr)
        , m_initialized(false)
    {
    }

    AudioDevice::~AudioDevice()
    {
        Shutdown();
    }

    bool AudioDevice::Initialize()
    {
        return Initialize(GetDefaultDeviceName());
    }

    bool AudioDevice::Initialize(const std::string& deviceName)
    {
        if (m_initialized) return true;

        // Open device
        ALCdevice* device = alcOpenDevice(deviceName.empty() ? nullptr : deviceName.c_str());
        if (!device) {
            USE_LOG_ERROR("AudioDevice: Failed to open audio device: %s",
                          deviceName.empty() ? "(default)" : deviceName.c_str());
            return false;
        }

        // Create context
        ALCcontext* context = alcCreateContext(device, nullptr);
        if (!context) {
            alcCloseDevice(device);
            USE_LOG_ERROR("AudioDevice: Failed to create OpenAL context");
            return false;
        }

        // Make context current
        alcMakeContextCurrent(context);

        // Check for errors
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            USE_LOG_ERROR("AudioDevice: OpenAL error during initialization: 0x%04X", error);
            alcDestroyContext(context);
            alcCloseDevice(device);
            return false;
        }

        m_device = device;
        m_context = context;
        m_initialized = true;

        USE_LOG_INFO("AudioDevice: Initialized using '%s'", GetDeviceName().c_str());
        return true;
    }

    void AudioDevice::Shutdown()
    {
        if (!m_initialized) return;

        alcMakeContextCurrent(nullptr);

        if (m_context) {
            alcDestroyContext((ALCcontext*)m_context);
            m_context = nullptr;
        }

        if (m_device) {
            alcCloseDevice((ALCdevice*)m_device);
            m_device = nullptr;
        }

        m_initialized = false;
        USE_LOG_INFO("AudioDevice: Shut down");
    }

    std::string AudioDevice::GetDeviceName() const
    {
        if (!m_initialized || !m_device) return "";
        const ALCchar* name = alcGetString((ALCdevice*)m_device, ALC_DEVICE_SPECIFIER);
        return name ? name : "";
    }

    std::string AudioDevice::GetVendor() const
    {
        if (!m_initialized) return "";
        const ALchar* vendor = alGetString(AL_VENDOR);
        return vendor ? vendor : "";
    }

    std::string AudioDevice::GetVersion() const
    {
        if (!m_initialized) return "";
        const ALchar* version = alGetString(AL_VERSION);
        return version ? version : "";
    }

    std::vector<std::string> AudioDevice::EnumerateDevices()
    {
        std::vector<std::string> devices;

        // Check if enumeration is supported
        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE) {
            const ALCchar* deviceList = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
            const ALCchar* device = deviceList;
            while (device && *device) {
                devices.push_back(device);
                device += strlen(device) + 1;
            }
        }

        return devices;
    }

    std::string AudioDevice::GetDefaultDeviceName()
    {
        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE) {
            const ALCchar* name = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
            if (name) return name;
        }
        return "";
    }

} // namespace USE