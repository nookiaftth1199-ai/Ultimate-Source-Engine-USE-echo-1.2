// ============================================================
// Ultimate Source Engine - Skeleton Implementation
// ============================================================

#include "stdafx.h"
#include "Skeleton.h"

namespace USE {

    Bone::Bone()
        : parentIndex(-1)
    {
    }

    Bone::Bone(const std::string& name, int parent)
        : name(name)
        , parentIndex(parent)
    {
    }

    // -----------------------------------------------------------------
    Skeleton::Skeleton()
    {
    }

    Skeleton::~Skeleton()
    {
    }

    void Skeleton::Clear()
    {
        m_bones.clear();
        m_nameToIndex.clear();
    }

    int Skeleton::AddBone(const Bone& bone)
    {
        int index = (int)m_bones.size();
        m_bones.push_back(bone);
        m_nameToIndex[bone.name] = index;
        return index;
    }

    Bone* Skeleton::GetBone(int index)
    {
        if (index >= 0 && index < (int)m_bones.size())
            return &m_bones[index];
        return nullptr;
    }

    const Bone* Skeleton::GetBone(int index) const
    {
        if (index >= 0 && index < (int)m_bones.size())
            return &m_bones[index];
        return nullptr;
    }

    int Skeleton::FindBoneIndex(const std::string& name) const
    {
        auto it = m_nameToIndex.find(name);
        return (it != m_nameToIndex.end()) ? it->second : -1;
    }

    void Skeleton::ComputeGlobalTransforms(const Transform* localTransforms,
                                           Matrix4* outGlobalTransforms) const
    {
        int n = GetBoneCount();
        // Compute in order of increasing depth (assuming bones are sorted by parent index)
        // We'll use iterative approach: for each bone, combine parent's global with its local.
        for (int i = 0; i < n; ++i) {
            int parent = m_bones[i].parentIndex;
            if (parent == -1) {
                outGlobalTransforms[i] = localTransforms[i].ToMatrix();
            } else {
                outGlobalTransforms[i] = outGlobalTransforms[parent] * localTransforms[i].ToMatrix();
            }
        }
    }

    void Skeleton::ComputeSkinningMatrices(const Matrix4* globalTransforms,
                                           Matrix4* outSkinningMatrices) const
    {
        int n = GetBoneCount();
        for (int i = 0; i < n; ++i) {
            outSkinningMatrices[i] = globalTransforms[i] * m_bones[i].inverseBindPose;
        }
    }

} // namespace USE