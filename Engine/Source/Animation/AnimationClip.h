// ============================================================
// Ultimate Source Engine - Animation Clip
// ============================================================
#pragma once

#include "Pose.h"
#include <string>
#include <vector>

namespace USE
{
	// A single keyframe for a bone.
	struct BoneKeyframe
	{
		float      time;
		Vector3    position;
		Quaternion rotation;
	};

	// Track for one bone.
	struct BoneTrack
	{
		std::vector<BoneKeyframe> keyframes;
	};

	class AnimationClip
	{
	public:
		AnimationClip();
		~AnimationClip();

		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		void SetDuration(float duration) { m_duration = duration; }
		float GetDuration() const { return m_duration; }

		void SetFrameRate(float fps) { m_frameRate = fps; }
		float GetFrameRate() const { return m_frameRate; }
		uint32_t GetFrameCount() const { return static_cast<uint32_t>(m_duration * m_frameRate) + 1; }

		void SetBoneCount(uint32_t count);
		uint32_t GetBoneCount() const { return static_cast<uint32_t>(m_tracks.size()); }

		// Add a keyframe for a specific bone.
		void AddKeyframe(uint32_t boneIndex, const BoneKeyframe& kf);

		// Evaluate the clip at a given time (in seconds).
		Pose Evaluate(float time) const;

	private:
		std::string m_name;
		float m_duration = 0.0f;
		float m_frameRate = 30.0f;
		std::vector<BoneTrack> m_tracks;
	};
}