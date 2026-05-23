// ============================================================
// Ultimate Source Engine - Audio Device
// ============================================================
//
// Low-level wrapper for the audio hardware (OpenAL).
// Manages device and context, provides device information,
// and ensures proper initialization/shutdown.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

namespace USE {

    class AudioDevice {
    public:
        AudioDevice();
        ~AudioDevice();

        // Initialize the default audio device. Returns true on success.
        bool Initialize();

        // Initialize a specific audio device by name.
        bool Initialize(const std::string& deviceName);

        // Shutdown the audio device.
        void Shutdown();

        // Check if device is valid.
        bool IsValid() const { return m_initialized; }

        // Get the name of the current device.
        std::string GetDeviceName() const;

        // Get the vendor string (OpenAL version, etc.)
        std::string GetVendor() const;

        // Get the version string.
        std::string GetVersion() const;

        // Static: enumerate all available audio devices.
        static std::vector<std::string> EnumerateDevices();

        // Static: get the name of the default device.
        static std::string GetDefaultDeviceName();

    private:
        void* m_device;   // ALCdevice*
        void* m_context;  // ALCcontext*
        bool  m_initialized;
    };

} // namespace USE