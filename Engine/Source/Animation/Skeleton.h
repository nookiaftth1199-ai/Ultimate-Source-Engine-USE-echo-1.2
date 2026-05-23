// ============================================================
// Ultimate Source Engine - Skeleton
// ============================================================
//
// Represents a skeleton for skinned mesh animation.
// Contains bones arranged in a hierarchy, with local transforms
// and inverse bind pose matrices.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Transform.h"
#include "Math/Matrix4.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace USE {

    struct Bone {
        std::string name;               // Bone name (for identification)
        int         parentIndex;         // Index of parent bone (-1 for root)
        Transform   localTransform;      // Transform relative to parent
        Matrix4     inverseBindPose;     // Inverse of global bind pose (used for skinning)

        Bone();
        Bone(const std::string& name, int parent);
    };

    class Skeleton {
    public:
        Skeleton();
        ~Skeleton();

        // Clear all bones
        void Clear();

        // Add a new bone (returns its index)
        int AddBone(const Bone& bone);

        // Access bones
        int GetBoneCount() const { return (int)m_bones.size(); }
        Bone* GetBone(int index);
        const Bone* GetBone(int index) const;

        // Find bone index by name (returns -1 if not found)
        int FindBoneIndex(const std::string& name) const;

        // Compute global transforms for all bones given an array of local transforms.
        // The input array 'localTransforms' must have the same size as bone count.
        // The result is stored in 'outGlobalTransforms' (also sized bone count).
        void ComputeGlobalTransforms(const Transform* localTransforms,
                                     Matrix4* outGlobalTransforms) const;

        // Compute skinning matrices (global * inverseBindPose) for all bones.
        // This is what you upload to the shader for skinned animation.
        void ComputeSkinningMatrices(const Matrix4* globalTransforms,
                                     Matrix4* outSkinningMatrices) const;

    private:
        std::vector<Bone> m_bones;
        std::unordered_map<std::string, int> m_nameToIndex;
    };

} // namespace USE