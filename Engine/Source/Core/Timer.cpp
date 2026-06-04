#include "stdafx.h"
#include "Timer.h"
#include <SDL.h>

namespace USE
{
	Timer::Timer() { Reset(); }

	void Timer::Reset()
	{
		m_startTime = SDL_GetPerformanceCounter();
		m_pausedTime = 0;
		m_paused = false;
		m_pausedAt = 0;
	}

	uint64_t Timer::GetElapsedTicks() const
	{
		if (m_paused) return m_pausedAt - m_startTime;
		return SDL_GetPerformanceCounter() - m_startTime - m_pausedTime;
	}

	void Timer::SleepMicroseconds(uint64_t microseconds)
	{
		SDL_Delay(static_cast<uint32_t>(microseconds / 1000));
	}

	void Timer::Pause()
	{
		if (!m_paused)
		{
			m_paused = true;
			m_pausedAt = SDL_GetPerformanceCounter();
		}
	}

	void Timer::Resume()
	{
		if (m_paused)
		{
			m_paused = false;
			m_pausedTime += SDL_GetPerformanceCounter() - m_pausedAt;
		}
	}
}