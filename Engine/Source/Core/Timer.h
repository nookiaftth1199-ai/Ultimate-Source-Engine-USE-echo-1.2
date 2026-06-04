// Timer.h
#pragma once

#include <cstdint>

namespace USE
{
	class Timer
	{
	public:
		Timer();
		void Reset();
		uint64_t GetElapsedTicks() const;
		static void SleepMicroseconds(uint64_t microseconds);
		void Pause();
		void Resume();

	private:
		uint64_t m_startTime = 0;
		uint64_t m_pausedTime = 0;
		uint64_t m_pausedAt = 0;
		bool    m_paused = false;
	};
}