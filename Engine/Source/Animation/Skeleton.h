// ============================================================
// Ultimate Source Engine - Skeleton
// ============================================================
#pragma once

#include "Bone.h"
#include <vector>
#include <unordered_map>

namespace USE
{
	class Skeleton
	{
	public:
		Skeleton() = default;
		~Skeleton() = default;

		void AddBone(const Bone& bone);
		uint32_t GetBoneCount() const { return static_cast<uint32_t>(m_bones.size()); }
		const Bone& GetBone(uint32_t index) const { return m_bones[index]; }
		Bone& GetBone(uint32_t index) { return m_bones[index]; }

		int32_t GetBoneIndex(const std::string& name) const;
		const std::string& GetBoneName(uint32_t index) const;

		// Returns the parent index for a bone, or -1.
		int32_t GetBoneParent(uint32_t index) const;
		const std::vector<Bone>& GetBones() const { return m_bones; }

	private:
		std::vector<Bone> m_bones;
		std::unordered_map<std::string, uint32_t> m_nameToIndex;
	};
}