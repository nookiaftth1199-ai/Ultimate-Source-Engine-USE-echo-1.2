#include "stdafx.h"
#include "Animator.h"
#include "Skeleton.h"
#include "Core/Logger.h"

namespace USE
{
	Animator::Animator() = default;
	Animator::~Animator() = default;

	void Animator::Initialize(Skeleton* skeleton)
	{
		m_skeleton = skeleton;
		if (skeleton)
			m_finalPose.SetBoneCount(skeleton->GetBoneCount());
	}

	int Animator::AddState(const std::string& name, AnimationClip* clip, bool looping, float speed)
	{
		AnimationState state;
		state.clip = clip;
		state.playing = false;   // not active initially
		state.looping = looping;
		state.speed = speed;
		m_states.push_back(state);
		return static_cast<int>(m_states.size()) - 1;
	}

	void Animator::SetState(const std::string& name, float blendTime)
	{
		for (int i = 0; i < static_cast<int>(m_states.size()); ++i)
		{
			if (m_states[i].clip && m_states[i].clip->GetName() == name)
			{
				SetState(i, blendTime);
				return;
			}
		}
	}

	void Animator::SetState(int index, float blendTime)
	{
		if (index < 0 || index >= static_cast<int>(m_states.size())) return;

		if (m_currentState == index) return;

		// Start blending from current pose
		m_nextState = index;
		m_blendDuration = blendTime;
		m_blendTime = 0.0f;

		// Stop previous state, start new one
		if (m_currentState >= 0)
			m_states[m_currentState].playing = false;
		m_states[index].playing = true;
		m_states[index].time = 0.0f;

		if (blendTime <= 0.0f)
		{
			// Immediate switch
			m_currentState = index;
			m_nextState = -1;
		}
	}

	void Animator::SetBlendTree(BlendTree* tree)
	{
		m_blendTree = tree;
	}

	void Animator::Update(float deltaTime)
	{
		if (!m_skeleton) return;

		// Update active states
		for (auto& state : m_states)
			if (state.playing) state.Update(deltaTime);

		// If a blend tree is set, use it instead of simple state blending
		if (m_blendTree)
		{
			m_blendTree->Update(deltaTime);
			m_finalPose = m_blendTree->GetResultPose();
			return;
		}

		// Simple state blending
		if (m_currentState >= 0 && m_currentState < static_cast<int>(m_states.size()))
		{
			AnimationState& active = m_states[m_currentState];
			if (active.clip)
			{
				Pose activePose = active.clip->Evaluate(active.time);
				if (m_nextState >= 0 && m_blendDuration > 0.0f)
				{
					// Blend between current and next
					AnimationState& next = m_states[m_nextState];
					Pose nextPose = next.clip ? next.clip->Evaluate(next.time) : activePose;
					m_blendTime += deltaTime;
					float t = std::min(m_blendTime / m_blendDuration, 1.0f);
					// Simple linear blend (positions only for demo)
					for (uint32_t b = 0; b < m_finalPose.GetLocalPosition(0).x || true; ++b) // dummy
					{
						// In real implementation, blend per bone
					}
					m_finalPose = activePose; // placeholder – in real code, blend between activePose and nextPose
					if (t >= 1.0f)
					{
						m_currentState = m_nextState;
						m_nextState = -1;
					}
				}
				else
				{
					m_finalPose = activePose;
				}
			}
		}
	}
}