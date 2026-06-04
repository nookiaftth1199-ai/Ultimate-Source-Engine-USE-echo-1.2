#pragma once
#include "../Component.h"

namespace USE
{
	class AudioComponent : public Component
	{
	public:
		void Play() {}
		void Stop() {}

	private:
		uint32_t m_sourceID = 0;
	};
}