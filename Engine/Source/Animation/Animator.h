// ============================================================
// Ultimate Source Engine - Animator
//============================================================
//
// Manages animation states and blends them to produce final
// bone transforms for a skeleton.
// ============================================================

#pragma once

#include "stdafx.h"
#include "AnimationState.h"
#include "Skeleton.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace USE {

    class Animator {
    public:
        explicit Animator(Skeleton* skeleton);
        ~Animator();

        // Add a new animation state from a clip. Returns the state index.
        int AddState(const std::string& name, AnimationClip* clip);

        // Get a state by name or index.
        AnimationState* GetState(const std::string& name);
        AnimationState* GetState(int index);
        int GetStateCount() const { return (int)m_states.size(); }

        // Play/stop/pause a state by index.
        void Play(int index);
        void Stop(int index);
        void Pause(int index);

        // Set weight for blending (0-1). Default weight is 1.
        void SetStateWeight(int index, float weight);

        // Update all playing states (advance time).
        void Update(float deltaTime);

        // Compute final bone transforms (global * inverse bind pose) for skinning.
        // The result is stored internally and can be retrieved via GetSkinningMatrices().
        void ComputeSkinningMatrices();

        // Get the array of skinning matrices (size = skeleton->GetBoneCount()).
        const Matrix4* GetSkinningMatrices() const { return m_skinningMatrices.data(); }

        // Get the array of final bone transforms (global matrices, not multiplied by inverse bind).
        const Matrix4* GetGlobalBoneMatrices() const { return m_globalMatrices.data(); }

    private:
        Skeleton*                    m_skeleton;
        std::vector<AnimationState>  m_states;
        std::unordered_map<std::string, int> m_nameToIndex;

        // Cached arrays (size = number of bones)
        std::vector<Transform>       m_blendedLocalTransforms; // per bone, blended
        std::vector<Matrix4>         m_globalMatrices;          // global matrices
        std::vector<Matrix4>         m_skinningMatrices;        // final skinning matrices

        // Blend function: combine multiple states into one local transform per bone.
        void BlendBoneTransforms(int boneIndex, Transform& outTransform) const;
    };

} // namespace USE