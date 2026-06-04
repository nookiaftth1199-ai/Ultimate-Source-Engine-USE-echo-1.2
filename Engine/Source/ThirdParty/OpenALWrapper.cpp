#include "stdafx.h"
#include "OpenALWrapper.h"
#include "Core/Logger.h"

// #define USE_OPENAL

#ifdef USE_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace USE
{
	bool OpenALWrapper::Init()
	{
#ifdef USE_OPENAL
		m_device = alcOpenDevice(nullptr);
		if (!m_device) return false;
		m_context = alcCreateContext(m_device, nullptr);
		if (!m_context) return false;
		alcMakeContextCurrent(m_context);
		return true;
#else
		USE_LOG_WARN("OpenALWrapper: OpenAL not enabled. Define USE_OPENAL to enable.");
		return false;
#endif
	}

	void OpenALWrapper::Shutdown()
	{
#ifdef USE_OPENAL
		if (m_context) { alcDestroyContext(m_context); m_context = nullptr; }
		if (m_device) { alcCloseDevice(m_device);    m_device = nullptr; }
#endif
	}

	void OpenALWrapper::PlaySound(uint32_t source) {}
	void OpenALWrapper::StopSound(uint32_t source) {}
	void OpenALWrapper::SetListenerPosition(float x, float y, float z) {}
}