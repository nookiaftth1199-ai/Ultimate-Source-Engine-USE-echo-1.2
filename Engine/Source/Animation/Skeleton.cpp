#include "stdafx.h"
#include "Skeleton.h"

namespace USE
{
	void Skeleton::AddBone(const Bone& bone)
	{
		uint32_t index = static_cast<uint32_t>(m_bones.size());
		m_bones.push_back(bone);
		m_nameToIndex[bone.name] = index;
	}

	int32_t Skeleton::GetBoneIndex(const std::string& name) const
	{
		auto it = m_nameToIndex.find(name);
		return (it != m_nameToIndex.end()) ? static_cast<int32_t>(it->second) : -1;
	}

	const std::string& Skeleton::GetBoneName(uint32_t index) const
	{
		static std::string empty;
		return (index < m_bones.size()) ? m_bones[index].name : empty;
	}

	int32_t Skeleton::GetBoneParent(uint32_t index) const
	{
		return (index < m_bones.size()) ? m_bones[index].parentIndex : -1;
	}
}