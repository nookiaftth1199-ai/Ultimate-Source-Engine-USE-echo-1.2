// ============================================================
// Ultimate Source Engine - Animation State
// ============================================================
#pragma once

#include "AnimationClip.h"

namespace USE
{
	class AnimationState
	{
	public:
		AnimationClip* clip = nullptr;
		float time = 0.0f;
		float speed = 1.0f;
		float weight = 1.0f;   // for blending
		bool  looping = true;
		bool  playing = true;

		void Update(float deltaTime)
		{
			if (playing && clip)
			{
				time += deltaTime * speed;
				if (looping && clip->GetDuration() > 0.0f)
				{
					time = std::fmod(time, clip->GetDuration());
					if (time < 0.0f) time += clip->GetDuration();
				}
			}
		}
	};
}