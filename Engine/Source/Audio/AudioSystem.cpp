// ============================================================
// Ultimate Source Engine - Audio System Implementation
// ============================================================

#include "stdafx.h"
#include "AudioSystem.h"
#include "OpenALAudioDevice.h"
#include "FMODAudioDevice.h"
#include "Core/Logger.h"

namespace USE
{
	AudioSystem::AudioSystem()
	{
		// Default to OpenAL
		m_device = std::make_unique<OpenALAudioDevice>();
	}

	AudioSystem::~AudioSystem() { Shutdown(); }

	bool AudioSystem::Initialize()
	{
		if (!m_device)
		{
			USE_LOG_ERROR("AudioSystem: No audio device set.");
			return false;
		}
		return m_device->Initialize();
	}

	void AudioSystem::Shutdown()
	{
		if (m_device)
			m_device->Shutdown();
	}

	void AudioSystem::Update(float /*deltaTime*/)
	{
		// No per‑frame updates needed for now.
	}

	uint32_t AudioSystem::PlaySound(const std::string& filePath, bool loop, float volume, float pitch)
	{
		if (!m_device) return 0;

		uint32_t buffer = m_device->LoadSound(filePath);
		if (!buffer) return 0;

		uint32_t source = m_device->CreateSource();
		if (!source)
		{
			m_device->UnloadSound(buffer);
			return 0;
		}

		m_device->SetVolume(source, volume);
		m_device->SetPitch(source, pitch);
		m_device->Play(source, buffer, loop);

		m_playingBuffers[source] = buffer;
		return source;
	}

	void AudioSystem::StopSound(uint32_t sourceHandle)
	{
		if (!m_device) return;

		auto it = m_playingBuffers.find(sourceHandle);
		if (it != m_playingBuffers.end())
		{
			m_device->Stop(sourceHandle);
			m_device->UnloadSound(it->second);
			m_device->DestroySource(sourceHandle);
			m_playingBuffers.erase(it);
		}
	}

	void AudioSystem::SetListenerPosition(float x, float y, float z)
	{
		if (m_device) m_device->SetListenerPosition(x, y, z);
	}

	void AudioSystem::SetListenerOrientation(const Vector3& forward, const Vector3& up)
	{
		if (m_device) m_device->SetListenerOrientation(forward, up);
	}

	void AudioSystem::SetMasterVolume(float volume)
	{
		if (m_device) m_device->SetMasterVolume(volume);
	}

	void AudioSystem::SetDevice(std::unique_ptr<IAudioDevice> device)
	{
		if (m_device) m_device->Shutdown();
		m_device = std::move(device);
		if (m_device) m_device->Initialize();
	}
}