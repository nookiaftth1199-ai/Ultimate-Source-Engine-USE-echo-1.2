// ============================================================
// Ultimate Source Engine - Animator Component
// ============================================================
#pragma once

#include "Pose.h"
#include "AnimationState.h"
#include "BlendTree.h"
#include <vector>
#include <string>
#include <functional>

namespace USE
{
	class Skeleton;

	class Animator
	{
	public:
		Animator();
		~Animator();

		void Initialize(Skeleton* skeleton);

		// Add a named state (e.g. "Walk", "Run")
		int AddState(const std::string& name, AnimationClip* clip,
			bool looping = true, float speed = 1.0f);

		// Switch to a state (immediate or with blend time)
		void SetState(const std::string& name, float blendTime = 0.0f);
		void SetState(int index, float blendTime = 0.0f);

		// Update and get the final blended pose.
		void Update(float deltaTime);
		const Pose& GetFinalPose() const { return m_finalPose; }

		// BlendTree support – for complex blending.
		void SetBlendTree(BlendTree* tree);

		// Callbacks for animation events.
		void SetOnEvent(std::function<void(const std::string&)> callback) { m_eventCallback = callback; }

	private:
		Skeleton*               m_skeleton = nullptr;
		std::vector<AnimationState> m_states;
		int m_currentState = -1;
		int m_nextState = -1;
		float m_blendTime = 0.0f;
		float m_blendDuration = 0.0f;

		Pose m_finalPose;
		Pose m_tempPose;
		BlendTree* m_blendTree = nullptr;

		std::function<void(const std::string&)> m_eventCallback;
	};
}