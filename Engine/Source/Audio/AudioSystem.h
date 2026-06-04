// ============================================================
// Ultimate Source Engine - Audio System
// ============================================================
#pragma once
#include "Audio/IAudioDevice.h"
#include <memory>
#include <string>
#include <cstdint>

namespace USE
{
	class AudioSystem
	{
	public:
		AudioSystem();
		~AudioSystem();

		bool Initialize();
		void Shutdown();
		void Update(float deltaTime);

		// Play a sound file (auto‑creates source, auto‑unloads when done).
		uint32_t PlaySound(const std::string& filePath, bool loop = false,
			float volume = 1.0f, float pitch = 1.0f);

		// Stop and destroy a playing sound.
		void StopSound(uint32_t sourceHandle);

		// Set global listener attributes (usually updated from camera).
		void SetListenerPosition(float x, float y, float z);
		void SetListenerOrientation(const Vector3& forward, const Vector3& up);
		void SetMasterVolume(float volume);

		// Change to a different backend (e.g., switch to FMOD when available).
		void SetDevice(std::unique_ptr<IAudioDevice> device);

	private:
		std::unique_ptr<IAudioDevice> m_device;
		// Map from source handle to buffer handle, so we can unload when stopped.
		std::unordered_map<uint32_t, uint32_t> m_playingBuffers;
		uint32_t m_nextSource = 1;
	};
}